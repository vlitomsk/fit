#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define sqr(x) ((x)*(x))
#define a 1

float rho(float x, float y) {
    //return 4.0f*(2.0f);
   return -a*(x+y);
}

float fresh(float x, float y) {
    return x+y;
//    return 0.0f;
}

#define X 2.0f
#define Y 2.0f

#define in 16
#define jn 16

#define fsx ((in)+2)
#define fsy ((jn)+2)

//#define far(layer,i,j) f[(layer)*fsx*fsy+(j)*fsx+i]
#define far(layer,i,j) f[layer][i][j]
#define far0(i,j) far(l0,(i),(j))
#define far1(i,j) far(l1,(i),(j))

void pmat(float m[2][fsx][fsy], int w, int h) {
    int i,j;
    for (j = 0; j < fsy; ++j) {
        for (i = 0; i < fsx; ++i) {
            printf("%.2f ", m[0][i][j]);
        }
        printf("\n");
    }
}

int main() {
    const float hx = X / (in+1);
    const float hy = Y / (jn+1);

    const float eps = 1e-5f;

    const float owx = powf(hx,2);
    const float owy = powf(hy,2);
    const float c = 2.0f/owx + 2.0f/owy + a;

    const int bytesz = fsx * fsy * 2 * sizeof(float);

/*    float *f = (float*)malloc(bytesz);
    if (!f) {
        perror("malloc");
        return 1;
    }*/
    float f[2][fsx][fsy];

    int l0 = 1;
    int l1 = 0;

    int i,j;

    /** init */
    for (i = 0; i < fsx; ++i) {
        for (j = 0 ; j < fsy; ++j) {
            if (i==0 || j==0 || i == fsx-1 || j==fsy-1) {
                f[0][i][j] = f[1][i][j] = fresh(i*hx, j*hy);
            } else {
                f[0][i][j] = f[1][i][j] = 0;
            }
        }
    }

    pmat(f, fsx, fsy);
    printf("\n");
    /** solution **/

    int lt_eps;
    int iter = 0;
    do {
        lt_eps = 1;
        l0 ^= 1;
        l1 ^= 1;
        for (i = 1; i < fsx-1; ++i) {
            for (j = 1; j < fsy-1; ++j) {
                const float old_val = f[l0][i][j];
/*                const float val_i = (far0(i-1,j) + far0(i+1,j)) * owx;
                const float val_j = (far0(i,j-1) + far0(i,j-1)) * owy;
                const float new_val = (val_i + val_j - owx*owy*rho(i * hx, j * hy)) / (2.0f * (owx+owy));
                far1(i,j) = new_val;*/
/*                const float new_val = 0.25f * (far0(i-1,j)+far0(i+1,j)+far0(i,j-1)+far0(i,j+1)) + hx*hx*0.25f*rho(i*hx,j*hy);
                far1(i,j) = new_val;*/
                const float val_i = (f[l0][i-1][j] + f[l0][i+1][j]) / owx;
                const float val_j = (f[l0][i][j-1] + f[l0][i][j+1]) / owy;
                const float new_val = (val_i + val_j - rho(i*hx, j*hy)) / c;
                f[l1][i][j] = new_val;

                if (lt_eps && fabs(new_val - old_val) > eps) {
                    lt_eps = 0;
                }
            }
        }

        ++iter;
    } while (iter <= 10000 && lt_eps == 0);

    /** test **/

    pmat(f, fsx, fsy);
    printf("\n");

    printf("iters: %d\n", iter);
    float maxdiff = -1.0f;
    for (i = 1; i < fsx-1; ++i) {
        for (j = 1; j < fsy-1; ++j) {
            float diff = fabs(fresh(i*hx,j*hx) - far1(i,j));
            if (diff > maxdiff)
                maxdiff = diff;
        }
    }
    printf("maxdiff: %f\n", maxdiff);

    //free(f);

    return 0;
}
