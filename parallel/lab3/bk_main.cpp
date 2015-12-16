#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>

/* Количество ячеек вдоль координат x, y, z */
#define in 20
#define jn 20
#define kn 20
#define a 1

double Fresh(double, double, double);
double Ro(double, double, double);
void Inic();

/* Выделение памяти для 3D пространства для текущей и предыдущей итерации */
double F[2][in+1][jn+1][kn+1];
double hx, hy, hz;

/* Функция определения точного решения */
inline double Fresh(double x,double y,double z) {
    return x + y + z;
}

/* Функция задания правой части уравнения */
inline double Ro(double x, double y, double z) {
    return -a * (x + y + z);
}

/* Подпрограмма инициализации границ 3D пространства */
void Inic() {
    int i, j, k;
    for (i = 0; i <= in; i++) {
        for (j = 0; j <= jn; j++) {
            for (k = 0; k <= kn; k++) {
                if(i && j && k && i != in && j != jn && k != kn) {
                    F[0][i][j][k] = 0;
                    F[1][i][j][k] = 0;
                } else {
                    F[0][i][j][k] = Fresh(i * hx, j * hy, k * hz);
                    F[1][i][j][k] = Fresh(i * hx, j * hy, k * hz);
                }
            }
        }
    }
}

int main() {
    double X, Y, Z;
    double max, N, t1, t2;
    double owx, owy, owz, c, e;
    double Fi, Fj, Fk, F1;

    int i, j, k, mi, mj, mk;
    int R, fl, fl1, fl2;
    int it,f, L0, L1;
    long int osdt;
    struct timeval tv1, tv2;
    it = 0;
    X = 2.0;
    Y = 2.0;
    Z = 2.0;
    e = 0.00001;
    L0 = 1;
    L1 = 0;
    /* Размеры шагов */
    hx = X / in;
    hy = Y / jn;
    hz = Z / kn;

    owx = pow(hx,2);
    owy = pow(hy,2);
    owz = pow(hz,2);
    c = 2 / owx + 2 / owy + 2 / owz + a;
    gettimeofday(&tv1, NULL);
    /* Инициализация границ пространства */
    Inic();
    /* Основной итерационный цикл */
    do {
        f = 1;
        L0 = 1 - L0;
        L1 = 1 - L1;
        for (i = 1; i < in; i++) {
            for (j = 1; j < jn; j++) {
                for(k = 1; k < kn; k++) {
                    Fi = (F[L0][i+1][j][k] + F[L0][i-1][j][k]) / owx;
                    Fj = (F[L0][i][j+1][k] + F[L0][i][j-1][k]) / owy;
                    Fk = (F[L0][i][j][k+1] + F[L0][i][j][k-1]) / owz;
                    F[L1][i][j][k] = (Fi + Fj + Fk - Ro(i*hx,j*hy,k*hz)) / c;
                    if (fabs(F[L1][i][j][k] - F[L0][i][j][k]) > e)
                        f = 0;
                }
            }
        }

        it++;
    } while (!f);

    gettimeofday(&tv2, NULL);
    osdt = (tv2.tv_sec - tv1.tv_sec)*1000000 + tv2.tv_usec - tv1.tv_usec;
    printf("in = %d ; iter = %d ; E = %f ; T = %ld\n", in, it, e, osdt);

    /* Нахождение максимального расхождения полученного приближенного решения
* и точного решения */
    max = 0.0;
    for(i = 1; i < in; i++) {
        for(j = 1; j < jn; j++) {
            for(k = 1; k < kn; k++) {
                if ( (F1 = fabs(F[L1][i][j][k] - Fresh(i*hx,j*hy,k*hz))) > max ) {
                    max = F1;
                    mi = i;
                    mj = j;
                    mk = k;
                }
            }
        }
    }

    printf("Max differ = %f\n in point(%d,%d,%d)\n", max, mi, mj, mk);

    return 0;
}
