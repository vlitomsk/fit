// orig http://ssd.sscc.ru/sites/default/files/content/attach/343/puassonv3dv2.pdf
#include <cuda.h>
#include <stdio.h>
#include <stdlib.h>
#include <cuda_profiler_api.h>

#define sqr(x) ((x)*(x))
#define cudaCheck \
    do { \
        cudaError_t err=cudaGetLastError(); \
        if( err != cudaSuccess ) { \
            printf(" cudaError = '%s' \n in '%s' %d \n", \
                    cudaGetErrorString(err), \
                    __FILE__, __LINE__ ); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

#define tid threadIdx
#define bid blockIdx
#define bdim blockDim

/* * * * * */
/* TASK PARAMS */
/* * * * * */

#define a 1.0f
/* Размеры сердцевины */
#define in 16
#define jn 16

#ifndef bsx
#define bsx 4
#endif

#ifndef bsy
#define bsy 4
#endif

const float X = 2.0f, Y = 2.0f;
const float hx = X/(in+1), hy = Y/(jn+1);
const float owx_rev = 1.0f/(hx*hx), owy_rev =1.0f/( hy*hy);
const float c_rev = 1.0f/(2.0f*owx_rev + 2.0f*owy_rev + a);

const float eps = 1e-5;

/* * * * * */
/* END TASK PARAMS */
/* * * * * */

void pmat(float *m, int w, int h) {
  for (int i = 0; i < h; ++i) {
    for (int j = 0; j < w; ++j) {
      printf("%.2f ", m[i*w + j]);
    }
    printf("\n");
  }
}


float Fresh_host(float x, float y) {
    return x+y;
}

__device__ float Fresh(float x, float y) {
    return x+y;
}

__device__ float Rho(float x, float y) {
    return -a*(x+y);
}

float Rho_host(float x, float y) {
    return -a*(x+y);
}

__device__ int lessthan_eps = 1; // 1 true, 0 false

#define cbsx (bsx+2)
#define cbsy (bsy+2)
#define fsx (in+2)
#define fsy (jn+2)
#define cbar(i,j) subcube[(j)*cbsx + (i)]
#define far(layer,i,j) fdev[fsx*fsy*(layer) + (j)*fsx + (i)]
#define far0(i,j) far(l0,(i),(j))
#define far1(i,j) far(l1,(i),(j))
#define far_h(layer,i,j) f_host[(layer)*fsx*fsy + (j)*fsx + (i)]

__global__ void initmat(float * fdev) {
    const int i = 1 + tid.x + bsx * bid.x,
              j = 1 + tid.y + bsy * bid.y;

    far(0,i,j) = far(1,i,j) = 0;

    if (i == 1) {
        far(0,0,j) = far(1,0,j) = Fresh(0*hx, j*hy);
        far(0,fsx-1,j) = far(1,fsx-1,j) = Fresh((fsx-1)*hx, j*hy);
    }

    if (j == 1) {
        far(0,i,0) = far(1,i,0) = Fresh(i*hx, 0*hy);
        far(0,i,fsy-1) = far(1,i,fsy-1) = Fresh(i*hx, (fsy-1)*hy);
    }
}

__global__ void dostep(float * fdev, int l0, int l1, bool cpsymbol) {
    __shared__ float subcube[cbsx*cbsy];
    __shared__ int lteps ;
    lteps = 1;
    // fij -- индекс потока в вычислительной сетке размером (in+2)*(jn+2) = fsx * fsy
    // cbij -- индекс потока в shared-области размером (bsx+2)*(bsy+2) = cbsx * cbsy
    const int cbi = 1 + tid.x,
              cbj = 1 + tid.y;
    const int fi = cbi + bsx * bid.x,
              fj = cbj + bsy * bid.y;

    cbar(cbi, cbj) = far0(fi, fj);
    const float old_val = cbar(cbi,cbj);

    if (tid.x <= 1) {
        int idx = tid.x * (bsx+1);
        cbar(idx,cbj) = far0(fi + idx - cbi, fj);
    }

    if (tid.y  <= 1) {
        int idx = tid.y * (bsy+1);
        cbar(cbi, idx) = far0(fi, fj + idx - cbj);
    }

    __syncthreads();
    const float Fival = (cbar(cbi+1, cbj) + cbar(cbi-1, cbj)) * owx_rev;
    const float Fjval = (cbar(cbi, cbj+1) + cbar(cbi, cbj-1)) * owy_rev;
    const float new_val = (Fival + Fjval - Rho(fi*hx, fj*hy)) * c_rev;
    far1(fi, fj) = new_val;

    if (cpsymbol) {
        if ( lteps && fabs(new_val - old_val) > eps )  {
            lteps = 0;
        }
        __syncthreads();
        if (tid.x == 0 && tid.y ==0 && !lteps) {
            lessthan_eps = 0;
        }
    }
}

int main(int argc, char ** argv) {
    cudaSetDevice(0); cudaCheck;
    cudaEvent_t ev_start, ev_end;
    cudaEventCreate(&ev_start); cudaCheck;
    cudaEventCreate(&ev_end); cudaCheck;

    /* * * * */

    int bytesz = 2 *fsx * fsy * sizeof(float);
    float * f_host = (float*) malloc(bytesz);
    float * f_dev;
    if (!f_host) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    cudaMalloc(&f_dev, bytesz); cudaCheck;


    /** DO ALL WORK HERE **/

    const bool profena = false;
    const int epsCheckFreq = 10;
    int lt_eps, iter = 0;

    dim3 gdim = dim3(in/bsx, jn/bsy, 1);
    dim3 bdim = dim3(bsx, bsy, 1);

    initmat <<< gdim, bdim >>> ( f_dev ); cudaCheck;
    cudaDeviceSynchronize(); cudaCheck;

    if (profena) {
        cudaProfilerStart(); cudaCheck;
    }

    cudaEventRecord(ev_start, 0); cudaCheck;
    int *cuda_lteps;
    cudaGetSymbolAddress((void**)&cuda_lteps, "lessthan_eps");
    do {
        bool cpsymbol = iter % epsCheckFreq == 0;
        if (cpsymbol) {
            lt_eps = 1;
            cudaMemcpy(cuda_lteps, &lt_eps, sizeof(int), cudaMemcpyHostToDevice); cudaCheck;
        }
        dostep <<< gdim, bdim >>> ( f_dev, iter&1, (iter&1)^1, cpsymbol ); cudaCheck;
        cudaDeviceSynchronize(); cudaCheck;
        if (cpsymbol) {
            cudaMemcpy(&lt_eps, cuda_lteps, sizeof(int), cudaMemcpyDeviceToHost); cudaCheck;
        }
        ++iter;
    } while (iter < 1000 && lt_eps == 0);

    cudaEventRecord(ev_end, 0); cudaCheck;

    if (profena) {
        cudaProfilerStop(); cudaCheck;
    }

    /** copy result to HOST and check */

    cudaMemcpy(f_host, f_dev, bytesz, cudaMemcpyDeviceToHost); cudaCheck;

    if( fsx<=32 && fsy<=32 ) {
        pmat(f_host, fsx,fsy);
        printf("\n\n");
    }

    float maxerr = 0.0f;
    int mi, mj;
    for (int i = 1; i < in + 1; ++i) {
        for (int j = 1; j < jn + 1; ++j) {
            float fl = fabs(far_h(0,i,j) - Fresh_host(i*hx, j*hy));
            if (fl > maxerr){
                maxerr = fl;
                mi = i;
                mj = j;
            }
        }
    }
    printf("iters: %3d; <%3d %3d> : maxerr: %f fresh = %f\n", iter, mi,mj, maxerr, Fresh_host( mi*hx, mj*hy ) );

    /* * * * */
    float time_took;
    cudaEventElapsedTime(&time_took, ev_start, ev_end); cudaCheck;
    printf("Time: %f\n", time_took);

    free(f_host);
    cudaFree(f_dev); cudaCheck;


    return EXIT_SUCCESS;
}

