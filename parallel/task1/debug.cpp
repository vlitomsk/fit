#include "debug.h"
#include <sys/types.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <openmpi/mpi.h>
#include <pthread.h>

int gdb_check() {
    int pid = fork();
    int status;
    int res;

    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        int ppid = getppid();

        /* Child */
        if (ptrace(PTRACE_ATTACH, ppid, NULL, NULL) == 0)  {
            /* Wait for the parent to stop and continue it */
            waitpid(ppid, NULL, 0);
            ptrace(PTRACE_CONT, NULL, NULL);

            /* Detach */
            ptrace(PTRACE_DETACH, getppid(), NULL, NULL);

            /* We were the tracers, so gdb is not present */
            res = 0;
        } else {
            /* Trace failed so gdb is present */
            res = 1;
        }
        exit(res);
    } else {
        waitpid(pid, &status, 0);
        res = WEXITSTATUS(status);
    }
    return res;
}

void* rungdb(void *ppid) {
    pid_t pid = *(pid_t*)ppid;
    char gdbarg[1024];
    char titlearg[256];
    int rank;

    memset(gdbarg, 0, 1024);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    sprintf(titlearg, "Rank %d", rank);
    sprintf(gdbarg, "gdb -q ");
    readlink("/proc/self/exe", gdbarg + strlen(gdbarg), 256);
    sprintf(gdbarg + strlen(gdbarg), " %d", pid);
    pid_t child = fork();
    if (child < 0) {
        perror("fork");
    } else if (child == 0)
        execl("/usr/bin/xterm", "xterm", "-title", titlearg, "-e", gdbarg, NULL);
    waitpid(child, NULL, 0);
}

void mpi_gdb() {
    if (gdb_check())
        return;
    int rank, i = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    pid_t pid = getpid();
    printf("PID (rank %d) %d ready for attach\n", rank, pid);

    /* fk dis shit */
    pthread_attr_t thread_attr;
    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    pthread_t thread;
    pthread_create(&thread, &thread_attr, rungdb, &pid);

    while (!gdb_check())
        usleep(500000);
}
