#include <mpi.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cmath>

using namespace std;

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

#define rootrank 0
#define max_iters 100000

#define a (-1)
#define eps 0.00001
#define X 2.0
#define Y 2.0
#define Z 2.0

int rank, commsize;
int zfrom, zto; // для декомпозиции на пр.параллелепипеды xsize*ysize*(zto-zfrom)
// размеры параллелепипеда:
int xsize, ysize, zsize; // только для текущего процесса
bool haszprev, hasznext; // существуют ли перекрытия с предыдущим (z-1) или следующим (z+1) блоком
double hx, hy, hz; // размеры шагов
double owx, owy, owz, c;

// для всей задачи
int g_xsize, g_ysize, g_zsize;
/*#define g_xsize 20
#define g_ysize 2000
#define g_zsize 20*/

inline double rho(double x, double y, double z) {
    return -a * (x + y + z);
}

inline double fresh(double x, double y, double z) {
    return x + y + z;
}

struct result_t {
    int found; // bool
    double time_mainloop; // секунды
    double time_loops; // секунды
    double maxabserror;
    double *error;
};

inline int offset4(int n, int x, int y, int z) {
    return n * xsize * ysize * zsize + (z - zfrom) * xsize * ysize + y * xsize + x;
}

#define oxy_offset4(n, z) offset4((n), 0, 0, (z))

#define isend_oxy(dst, n, z, F, mpirequestptr) MPI_Isend((F) + oxy_offset4((n), (z)), xsize * ysize, MPI_DOUBLE, (dst), 0, MPI_COMM_WORLD, mpirequestptr)

#define irecv_oxy(src, n, z, F, mpirequestptr) MPI_Irecv((F) + oxy_offset4((n), (z)), xsize * ysize, MPI_DOUBLE, (src), 0, MPI_COMM_WORLD, mpirequestptr)

int calclayer(double *F, int L0, int L1, int z, double &maxabserror) {
    int found = 1;
    for (int j = 1; j < ysize - 1; ++j) {
        for (int i = 1; i < xsize - 1; ++i) {
            double Fi = (F[offset4(L0, i + 1, j, z)] + F[offset4(L0, i - 1, j, z)]) / owx;
            double Fj = (F[offset4(L0, i, j + 1, z)] + F[offset4(L0, i, j - 1, z)]) / owy;
            double Fk = (F[offset4(L0, i, j, z + 1)] + F[offset4(L0, i, j, z - 1)]) / owz;

            F[offset4(L1, i, j, z)] = (Fi + Fj + Fk - rho(i * hx, j * hy, z * hz)) / c;
            if (fabs(F[offset4(L1, i, j, z)] - F[offset4(L0, i, j, z)]) > eps)
                found = 0;
            maxabserror = max(maxabserror, fabs(F[offset4(L1, i, j, z)] - fresh(i * hx, j * hy, z * hz)));
        }
    }
    return found;
}

void Inic(double *F) {
    for (int k = zfrom; k < zto; ++k) {
        for (int j = 0; j < ysize; ++j) {
            for (int i = 0; i < xsize; ++i) {
                if (i && j && k != zfrom
                   && i != xsize - 1 && j != ysize - 1 && k != zto - 1)
                {
                    // не на глобальной ранице
                    F[offset4(0, i, j, k)] = 0;
                    F[offset4(1, i, j, k)] = 0;
                } else {
                    // на глобальной границе
                    F[offset4(0, i, j, k)] = fresh(i * hx, j * hy, k * hz);
                    F[offset4(1, i, j, k)] = fresh(i * hx, j * hy, k * hz);
                }
            }
        }
    }
}

int piece_start_idx(int dim, int rk, int comm_size) {
    const int eff_comm_size = min(dim, comm_size);
    const int rest = dim % eff_comm_size;
    return rk < rest ? (dim / comm_size + 1) * rk
                       : rest + rk * (dim / eff_comm_size);
}

result_t run() {
    result_t res = {
        .found = false,
        .time_mainloop = 0.0,
        .time_loops = 0.0,
        .maxabserror = 0,
        .error = NULL
    };
    /* считаем параметры для текущего процесса */
    hx = X / g_xsize;
    hy = Y / g_ysize;
    hz = Z / g_zsize;
    //assert(g_zsize % commsize == 0);
    xsize = g_xsize;
    ysize = g_ysize;
    haszprev = rank > 0;
    hasznext = rank < commsize - 1;
    const int kfrom = piece_start_idx(g_zsize, rank, commsize);
    const int kto = piece_start_idx(g_zsize, rank + 1, commsize);
    zfrom = kfrom - (haszprev ? 1 : 0); // inclusive
    zto = kto + (hasznext ? 1 : 0); // exclusive
    zsize = zto - zfrom;
    /* посчитали параметры для текущего процесса */

    double *F = new (std::nothrow) double[2 * xsize * ysize * zsize];
    if (!F) {
        std::cerr << "Can't allocate memory" << std::endl;
        exit(EXIT_FAILURE);
    }

    /* поехали! */
    res.time_loops = MPI_Wtime();
    /* заполняем начальные условия */
    Inic(F);

    // изначально у всех процессов есть соседние плоскости

    /* основной цикл */
    owx = hx * hx;
    owy = hy * hy;
    owz = hz * hz;
    c = 2.0 / owx + 2.0 / owy + 2.0 / owz + a;

    res.time_mainloop = MPI_Wtime();
    res.found = 0;
    cout << "rank " << rank << "; zfrom,zto  " << zfrom << "," << zto << endl;
    double loc_maxabserror;

    for (int iters = 0; !res.found && iters < max_iters; ++iters) {
        loc_maxabserror = 0.0;
        const int L0 = (iters & 1);
        const int L1 = L0 ^ 1;
        int converges = 1; // сходится ли решение в текущем блоке

        // . в этой точке считаем, что L0 уже посчитан

        // считаем границы и запускаем асинхронную пересылку
        MPI_Request rx_rq1, rx_rq2, tx_rq1, tx_rq2;
        rx_rq1 = rx_rq2 = MPI_REQUEST_NULL;

        if (iters && haszprev) {
            //cout << "calc prev" << endl;
            converges &= calclayer(F, L0, L1, zfrom + 1, loc_maxabserror); // нашу границу считаем
            isend_oxy(rank - 1, L1, zfrom + 1, F, &tx_rq1);
            irecv_oxy(rank - 1, L1, zfrom, F, &rx_rq1);
        }
        if (iters && hasznext) {
            //cout << "calc next" << endl;
            converges &= calclayer(F, L0, L1, zto - 2, loc_maxabserror); // нашу границу считаем
            isend_oxy(rank + 1, L1, zto - 2, F, &tx_rq2);
            irecv_oxy(rank + 1, L1, zto - 1, F, &rx_rq1);
        }

        // пока идут данные, считаем всё остальное

        for (int k = kfrom + 1; k < kto - 1; ++k) {
            if ((k == zfrom + 1 && haszprev) || (k == zto - 2 && hasznext))
                continue;
            converges &= calclayer(F, L0, L1, k, loc_maxabserror);
        }

        // ждем, пока процессы не поделятся границами
        MPI_Status rx_st1, rx_st2;
        MPI_Wait(&rx_rq1, &rx_st1);
        MPI_Wait(&rx_rq2, &rx_st2);

        //MPI_Barrier(MPI_COMM_WORLD);
        MPI_Allreduce(&converges, &res.found, 1, MPI_INT, MPI_BAND, MPI_COMM_WORLD);
        //cout << "iter " << res.iters << " done\n";
        //cout << "found: " << res.found << endl;
    }
    MPI_Reduce(&loc_maxabserror, &res.maxabserror, 1, MPI_DOUBLE, MPI_MAX, rootrank, MPI_COMM_WORLD);

    double endtime = MPI_Wtime();
    res.time_loops = endtime - res.time_loops;
    res.time_mainloop = endtime - res.time_mainloop;

    /* освобождаем ресурсы */
    delete[] F;

    return res;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <size>" << endl;
        return EXIT_FAILURE;
    }
    g_xsize = g_ysize = g_zsize = atoi(argv[1]);
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &commsize);

    result_t result = run();

    MPI_Finalize();

    if (rank == rootrank) {
        cout << "Done calculations" << endl
             << "Found: " << result.found << endl
             << "Maxabserror: " << result.maxabserror << endl
             << "time_loops: " << result.time_loops << endl
             << "time_mainloop: " << result.time_mainloop << endl;
    }

    return EXIT_SUCCESS;
}
