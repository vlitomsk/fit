#include <mpi.h>
#include <pthread.h>
#include <iostream>
#include <vector>
#include <queue>
#include <array>
#include <cstdlib>
#include <cstddef>
#include <stdexcept>
#include <cmath>
#include <cassert>
#include <numeric>
#include <algorithm>
#include <time.h>

using namespace std;

#define LOG cout << "[" << comm_rank << "]"

/* типы */
struct Task {
    int repeat_num;
    Task() : repeat_num(0) {}
    Task(int repeat_num) : repeat_num(repeat_num) {}
    Task(const Task &t) = default;
    ~Task() = default;
    const Task& operator=(const Task &t) {
        if (&t != this) {
            repeat_num = t.repeat_num;
        }
    }

    void run() {
        struct timespec ts = {
            .tv_sec = 0,
            .tv_nsec = 1000 * 1000 * 100 * ((repeat_num + 1))
        };
        nanosleep(&ts, nullptr);
    }

    static void create_MPI_datatype(MPI_Datatype &tp) {
        static int blocklengths[1] = { 1 };
        static MPI_Datatype types[1] = { MPI_INT };
        static MPI_Aint offsets[1] = { offsetof(Task, repeat_num) };

        MPI_Type_create_struct(1, blocklengths, offsets, types, &tp);
        MPI_Type_commit(&tp);
    }

    static void free_MPI_datatype(MPI_Datatype &tp) {
        MPI_Type_free(&tp);
    }
};

struct TaskList {
    TaskList()
        : mutexes({{ &mut_data }}),
          conds({ &cv_nonempty, &cv_empty })
    {
        for (auto mut_ptr : mutexes)
            pthread_mutex_init(mut_ptr, nullptr);
        for (auto cond_ptr : conds)
            pthread_cond_init(cond_ptr, nullptr);
    }

    ~TaskList() {
        for (auto mut_ptr : mutexes)
            pthread_mutex_destroy(mut_ptr);
        for (auto cond_ptr : conds)
            pthread_cond_destroy(cond_ptr);
    }

    void push(const Task &t) {
        tqueue.push(t);
        if (tqueue.size() == 1)
            pthread_cond_signal(&cv_nonempty);
    }

    void pop(Task &out) {
        out = tqueue.front();
        tqueue.pop();
        if (tqueue.size() == 0)
            pthread_cond_signal(&cv_empty);
    }

    void generate(std::size_t listLen, int rank) {
        while (tqueue.size())
            tqueue.pop();

        for (; listLen != 0; --listLen)
            tqueue.push(Task(rank));
    }

    inline std::size_t size() {
        return tqueue.size();
    }

    inline void lock() {
        pthread_mutex_lock(&mut_data);
    }

    inline void unlock() {
        pthread_mutex_unlock(&mut_data);
    }

    inline void wait4empty() {
        pthread_cond_wait(&cv_empty, &mut_data);
    }

    inline void wait4nempty() {
        pthread_cond_wait(&cv_nonempty, &mut_data);
    }
private:
    std::array<pthread_mutex_t *, 1> mutexes;
    std::array<pthread_cond_t *, 2> conds;
    pthread_mutex_t mut_data;
    pthread_cond_t cv_nonempty, cv_empty;
    std::queue<Task> tqueue;
};

void * thread_work(void *);
void * thread_reply(void *);
void * thread_download(void *);

/* глобальные переменные не важные */
pthread_t thrs[3];
void * (*thfns[3])(void *) = { thread_work, thread_reply, thread_download };

/* константы */
const int root_node = 0;

/* глобальные переменные ВАЖНЫЕ */
int comm_rank, comm_size;
MPI_Comm ctl_comm, data_comm;
MPI_Datatype mpi_task_type;
TaskList task_list;
Task active_task;
std::vector<bool> finished;
int working_processes;

const int TAG_CTL_ASK = 1;
const int TAG_RETCOUNT = 2;
const int TAG_CTL_GIMME = 3;
const int TAG_CTL_FINISH = 6;
const int TAG_TASK_COUNT = 4;
const int TAG_TASKS = 5;

/* отправляем всем оповещение о том, что у нас закончились задания */
void send_ctl_ask() {
    int stub;
    for (int rk = 0; rk < comm_size; ++rk) {
        if (rk != comm_rank && !finished[rk])
            MPI_Send(&stub, 1, MPI_INT, rk, TAG_CTL_ASK, ctl_comm);
    }
}

/* отправляем всем оповещение о том, что мы больше не играем */
void send_ctl_finish() {
    int stub;
    for (int rk = 0; rk < comm_size; ++rk) {
        MPI_Send(&stub, 1, MPI_INT, rk, TAG_CTL_FINISH, ctl_comm);
    }
}

/* получаем числа оставшихся заданий на процессах */
/* эти числа могут быть уже немного протухшими */
void recv_ctl_retcounts(std::vector<int> &replies) {
    MPI_Status st;
    assert(static_cast<int>(replies.size()) == comm_size);
    std::fill(replies.begin(), replies.end(), 0);
    for (int rk = 0; rk < comm_size; ++rk)
        if (rk != comm_rank && !finished[rk])
            MPI_Recv(&replies[rk], 1, MPI_INT, rk, TAG_RETCOUNT, data_comm, &st);
}

/* вычисляем, сколько задач с какого процесса желаем забрать */
int calc_gimmes(const std::vector<int> &replies, std::vector<int> &gimmes) {
    int total_gimmes = 0;
    assert(static_cast<int>(replies.size()) == comm_size);
    assert(static_cast<int>(gimmes.size()) == comm_size);
    //std::fill(gimmes.begin(), gimmes.end(), 0);
    const int average = std::accumulate(replies.begin(), replies.end(), 0) / replies.size();
    std::transform(replies.begin(), replies.end(), gimmes.begin(), [&](int reply) {
        const int inc = (!reply || reply <= average) ? 0 : reply - average;
        total_gimmes += inc;
        return inc;
    });
    return total_gimmes;
}

/* посылаем наши желания другим процессам */
void send_ctl_gimmes(std::vector<int> &gimmes) {
    assert(static_cast<int>(gimmes.size()) == comm_size);
    for (int rk = 0; rk < comm_size; ++rk)
        if (rk != comm_rank && !finished[rk] && gimmes[rk])
            MPI_Send(&gimmes[rk], 1, MPI_INT, rk, TAG_CTL_GIMME, ctl_comm);
}

/* принимаем задачи согласно нашим желаниям и складываем в task_list */
/* не обязательно ВСЕ наши желания будут удовлетворены */
void recv_ctl_tasks(TaskList &locked_tl) {
    Task tmp;
    int cnt;
    MPI_Status st;
    for (int rk = 0; rk < comm_size; ++rk) {
        if (rk != comm_rank && !finished[rk]) {
            MPI_Recv(&cnt, 1, MPI_INT, rk, TAG_TASK_COUNT, data_comm, &st);
            for (; cnt > 0; --cnt) {
                MPI_Recv(&tmp, 1, mpi_task_type, rk, TAG_TASKS, data_comm, &st);
                locked_tl.push(tmp);
            }
        }
    }
}

/* основной замес -- в этих трёх функциях */

void * thread_work(void *) {
    bool has_task;
    while (!finished[comm_rank]) {
        task_list.lock();
//        LOG << "lock oK" << endl;
        if (task_list.size() == 0) {
  //          LOG << "waiting for task" << endl;
            task_list.wait4nempty();
    //        LOG << "wait done" << endl;
        }
        if (finished[comm_rank]) {
            task_list.unlock();
            task_list.pop(active_task);
            break;
        }
        task_list.pop(active_task);
        LOG << "task POP; tasks left = " << task_list.size() << endl;
        //has_task = task_list.size()
        task_list.unlock();

        //LOG << "start task.." << endl;
        active_task.run();
        LOG << "task finished.." << endl;
    }
//    LOG << "THREAD work finished" << endl;
}

void * thread_reply(void *) {
    int ctl_msg;
    MPI_Status st;
    MPI_Request req;

    while (working_processes > 0) {
        MPI_Recv(&ctl_msg, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, ctl_comm, &st);
        const int asker = st.MPI_SOURCE;
        const int tag = st.MPI_TAG;
        //LOG << "replying to asker " << asker << " with tag " << tag << endl;
        if (tag == TAG_CTL_ASK) {
            LOG << "(reply ask)" << endl;
            /* у нас запросили количество оставшихся задач */
            /* просто отправляем его */
            task_list.lock();
            int sz =  task_list.size() <= 0 ? 0 : task_list.size() - 1;
            task_list.unlock();
            MPI_Send(&sz, 1, MPI_INT, asker, TAG_RETCOUNT, data_comm);
            LOG << "DONE (reply ask)" << endl;
        } else if (tag == TAG_CTL_GIMME) {
            LOG << "(reply gimme " << ctl_msg << ")" << endl;
            /* у нас запросили ctl_msg задач */
            /* мы отправляем min(ctl_msg, task_list.size() - 1) (последнюю в списке оставляем себе) */
            task_list.lock();
            int tcount = std::min(ctl_msg, static_cast<int>(task_list.size()) - 1);
            MPI_Send(&tcount, 1, MPI_INT, asker, TAG_TASK_COUNT, data_comm);
            for (int to_send = tcount; to_send > 0; --to_send) {
                Task tmp;
                task_list.pop(tmp);
                MPI_Send(&tmp, 1, mpi_task_type, asker, TAG_TASKS, data_comm);
            }
            task_list.unlock();
            LOG << "DONE (reply gimme " << ctl_msg << ")" << endl;
        } else if (tag == TAG_CTL_FINISH) {
            LOG << "(reply finish)" << endl;
            finished[asker] = true;
            --working_processes;
            LOG << "DONE (reply finish)" << endl;
        }
    }
//    LOG << "THREAD reply finished" << endl;
}

void * thread_download(void *) {
    std::vector<int> replies(comm_size);
    std::vector<int> gimmes(comm_size);
    int total_gimmes = 1; /* сколько задач нам пришло в крайний раз */

    while (1) {
        LOG << "download thread waiting.." << endl;
        //abort();
        task_list.lock();
        if (task_list.size() != 0)
            task_list.wait4empty();

        LOG << "download thread GO GO GO!" << endl;

        send_ctl_ask(); /* отправляем всем оповещение о том, что у нас закончились задания */

        recv_ctl_retcounts(replies); /* получаем числа оставшихся заданий на процессах */
                                     /* эти числа могут быть уже немного протухшими */
        LOG << "RETCOUNTS: ";
        for (auto i : replies)
            cout << i << " ";
        cout << endl;

        total_gimmes = calc_gimmes(replies, gimmes); /* вычисляем, сколько задач с какого процесса желаем забрать */
        if (total_gimmes <= 0) {
            task_list.unlock();
            break;
        }

        send_ctl_gimmes(gimmes); /* посылаем наши желания другим процессам */

        recv_ctl_tasks(task_list); /* принимаем задачи согласно нашим желаниям и складываем в task_list */
                                   /* не обязательно ВСЕ наши желания будут удовлетворены */

        task_list.unlock();
    }

//    LOG << "THREAD download finished" << endl;
    send_ctl_finish(); /* отправляем всем оповещение о том, что мы вычислили все задания и больше вычислять не будем */
    finished[comm_rank] = true;

    task_list.lock();
    if (task_list.size() != 0)
        task_list.wait4empty();
    task_list.unlock();

    task_list.lock();
    task_list.push(Task());
    task_list.unlock();
}

/********************/

int main(int argc, char **argv) {
    int prov;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &prov);
    if (prov != MPI_THREAD_MULTIPLE) {
        cerr << "MPI_THREAD_MULTIPLE is not supported in this implementation of MPI" << endl;
        throw std::runtime_error("bad args");
    }
    /* создаем коммуникатор */
    MPI_Comm_dup(MPI_COMM_WORLD, &ctl_comm);
    MPI_Comm_dup(MPI_COMM_WORLD, &data_comm);

    MPI_Comm_rank(ctl_comm, &comm_rank);
    MPI_Comm_size(ctl_comm, &comm_size);
    /* создаем MPI тип данных для структуры Task */
    Task::create_MPI_datatype(mpi_task_type);
    finished = std::vector<bool>(comm_size, false);
    working_processes = comm_size;

    /* генерируем список заданий */
    task_list.generate(10, comm_rank);

    double starttime = MPI_Wtime();
    pthread_attr_t attr;
    if (pthread_attr_init(&attr)) {
        perror("pthread_attr_init");
        throw std::runtime_error("pthreads");
    }

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) {
        perror("pthread_attr_setdetachstate");
        throw std::runtime_error("pthreads");
    }

    for (int i = 0; i < 3; ++i) {
        if (pthread_create(thrs + i, &attr, thfns[i], nullptr)) {
            perror("pthread_create");
            throw std::runtime_error("pthreads");
        }
    }

    for (int i = 0; i < 3; ++i) {
        if (pthread_join(thrs[i], nullptr)) {
            perror("pthread_join");
            throw std::runtime_error("pthreads");
        }
    }
    double endtime = MPI_Wtime();
    if (comm_rank == 0) {
      cout << "TIMETIME: " << (endtime - starttime) << endl;
    }
    Task::free_MPI_datatype(mpi_task_type);
    MPI_Finalize();


    return EXIT_SUCCESS;
}
