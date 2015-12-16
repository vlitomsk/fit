#include <iostream>
#include <utility>
#include <omp.h>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <time.h>

/* one big section */

using namespace std;

const size_t max_iters = 4;
const double eps = 1e-5;
int threads;

void read(size_t N, double* &bvec, double* &amat) {
    //N = 20128;
    //N = 4;
    bvec = new double[N];
    amat = new double[N * N];
    for (size_t i = 0; i < N; ++i) {
        bvec[i] = N + 1;
        for (size_t j = 0; j < N; ++j)
            amat[i * N + j] = (i == j) ? 2 : 1;
    }
}

void solve(size_t N, double* bvec, double* amat, double* &x, size_t &iters, bool &found) {
    x = new double[3 * N];
    double *y = x + N, *Ay = x + 2 * N;

    double *pdot_y_Ay = new double[3 * threads];
    double *pdot_Ay_Ay = pdot_y_Ay + threads,
           *pdot_y_y = pdot_Ay_Ay + threads,
           dot_y_Ay, dot_Ay_Ay, dot_y_y, bnorm;
    double tau;

    bnorm = 0.0;
    memset(pdot_y_y, 0, sizeof(double) * threads);
    #pragma omp parallel
    {
        int tIdx = omp_get_thread_num();
        #pragma omp for
        for (size_t i = 0; i < N; ++i)
            pdot_y_y[tIdx] += bvec[i] * bvec[i];
    }
    for (int i = 0; i < threads; ++i)
        bnorm += pdot_y_y[i];
    bnorm = sqrt(bnorm);

    for (size_t i = 0; i < N; ++i)
        x[i] = 34;

    #pragma omp parallel
    {
        const int tIdx = omp_get_thread_num();

        for (iters = 0; !found && iters < max_iters; ++iters) {
            if (iters != 0) {
                #pragma omp for
                for (size_t i = 0; i < N; ++i)
                    x[i] -= tau * y[i];
            }

            #pragma omp single
            {
                memset(pdot_y_Ay, 0, sizeof(double) * threads * 3);
            }

            #pragma omp for
            for (size_t i = 0; i < N; ++i) { /* y = Ax-b */
                y[i] = 0;
                for (size_t j = 0; j < N; ++j)
                    y[i] += amat[i * N + j] * x[j];
                y[i] -= bvec[i];
                pdot_y_y[tIdx] += y[i] * y[i];
            }

            #pragma omp for
            for (size_t i = 0; i < N; ++i) {
                Ay[i] = 0;
                for (size_t j = 0; j < N; ++j)
                    Ay[i] += amat[i * N + j] * y[j];
                pdot_Ay_Ay[tIdx] += Ay[i] * Ay[i];
                pdot_y_Ay[tIdx] += y[i] * Ay[i];
            }

            #pragma omp barrier
            #pragma omp sections
            {
                #pragma omp section
                {
                    dot_y_y = 0.0;
                    for (size_t i = 0; i < threads; ++i)
                        dot_y_y += pdot_y_y[i];
                }

                #pragma omp section
                {
                    dot_y_Ay = 0.0;
                    for (size_t i = 0; i < threads; ++i)
                        dot_y_Ay += pdot_y_Ay[i];
                }

                #pragma omp section
                {
                    dot_Ay_Ay = 0.0;
                    for (size_t i = 0; i < threads; ++i)
                        dot_Ay_Ay += pdot_Ay_Ay[i];
                }
            }

            #pragma omp single
            {
                tau = dot_y_Ay / dot_Ay_Ay;
                found = sqrt(dot_y_y) / bnorm < eps;
            }
        }
    }
    delete[] pdot_y_Ay;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " num_Threads size" << endl;
        return 1;
    }
    size_t N, iters;
    double *bvec, *amat, *x;
    bool found;


    threads = atoi(argv[1]);
    N = atoi(argv[2]);
    omp_set_num_threads(threads);

    read(N, bvec, amat);
    cout << "N = " << N << endl;
    struct timespec tbegin, tend;
    clock_gettime(CLOCK_MONOTONIC_RAW, &tbegin);
    solve(N, bvec, amat, x, iters, found);
    clock_gettime(CLOCK_MONOTONIC_RAW, &tend);

    if (found) {
        /*cout << "Found solution: ";
        for (size_t i = 0; i < N; ++i)
            cout << x[i] << " " << endl;*/
        cout << "Calculation time: " << (((double)tend.tv_sec - (double)tbegin.tv_sec) * 1e3 + ((double)tend.tv_nsec - (double)tbegin.tv_nsec) * 1e-6) << " ms" << endl;
    }

    delete[] bvec;
    delete[] amat;
    delete[] x;

    return 0;
}

