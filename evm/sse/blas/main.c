#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <cblas.h>

typedef enum {false, true} bool;

void allocMatrix(float **mat, int n, bool fillZeros) {
    if (!fillZeros)
        *mat = (float*)malloc(sizeof(float) * n * n);
    else
        *mat = (float*)calloc(n * n, sizeof(float));
}

void freeMatrix(float *mat, int n) {
    free(mat);
}

void genRandMatrix(float **mat, int n) {
    allocMatrix(mat, n, true);
    int i, j;
    for (i = 0; i < n; ++i) 
        for (j = i; j < n; ++j)
            (*mat)[i * n + j] = rand() % 100;
}

void genIdMatrix(float **mat, int n) {
    allocMatrix(mat, n, true);
    int i;
    for (i = 0; i < n; ++i)
        (*mat)[i * n + i] = 1;
}

void copyMatrix(float *dst, float *src, int n) {
    memcpy(dst, src, sizeof(float) * n * n);
}

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

// ||A||1
float calcMaxColSum(float *mat, int n) {
    float maxSum = -1.0f;
    int i;
    for (i = 0; i < n; ++i) {
        maxSum = max(maxSum, cblas_sasum(n, mat + i, n));
    }
    return maxSum;
}

// ||A||infty
float calcMacRowSum(float *mat, int n) {
    float maxSum = -1.0f;
    int i;
    for (i = 0; i < n; ++i) {
        maxSum = max(maxSum, cblas_sasum(n, mat + n * i, 1));
    }
    return maxSum;
}

void printMatrix(float *mat, int n) {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            printf("%f\t", mat[i * n + j]);
        }
        putchar('\n');
    }
}

void transposeMatrix(float *matIn, float *matOut, int n) {
    int i, j;

    if (matIn != matOut) {
        for (i = 0; i < n; ++i) {
            for (j = 0; j < n; ++j) {
                matOut[i * n + j] = matIn[j * n + i];
            }
        }
    } else {
        float tmp;
        for (i = 0; i < n; ++i) {
            for (j = i + 1; j < n; ++j) {
                tmp = matIn[i * n + j];
                matIn[i * n + j] = matIn[j * n + i];
                matIn[j * n + i] = tmp;
            }
        }
    }
}

void matrixReverse(float *matA, float *matArev, int N, int M) {
    float *matR, *matI, *matTmp;
    genIdMatrix(&matR, N);
    genIdMatrix(&matI, N);

    float coefB = 1.0f / (calcMaxColSum(matA, N) * calcMacRowSum(matA, N));
    cblas_sgemm(CblasRowMajor, CblasTrans, CblasNoTrans, N, N, N, -coefB, matA, N, matA, N, 1.0, matR, N);
    allocMatrix(&matTmp, N, false);

    int i;
    transposeMatrix(matR, matR, N);
    for (i = 0; i < M; ++i) {
        if (!(i & 1)) {
            cblas_saxpy(N * N, 1.0f, matI, 1, matArev, 1);
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, N, N, N, 1.0, matI, N, matR, N, 0.0, matTmp, N);
        } else {
            cblas_saxpy(N * N, 1.0f, matTmp, 1, matArev, 1);
            cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, N, N, N, 1.0, matTmp, N, matR, N, 0.0, matI, N);
        }
    }
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasTrans, N, N, N, coefB, matArev, N, matA, N, 0.0, matTmp, N);
    copyMatrix(matArev, matTmp, N);
    freeMatrix(matR, N);
    freeMatrix(matI, N);
}

int main(int argc, char *argv[]) {
    int i, j;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s N_matrix_size M_iters\n", argv[0]);
        exit(1);
    }
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    float *matA, *matArev;
    genRandMatrix(&matA, N);
//    printf("Matrix A: \n");
//    printMatrix(matA, N);

    allocMatrix(&matArev, N, true);
    matrixReverse(matA, matArev, N, M);

//    printf("Reverse A: \n");
//    printMatrix(matArev, N);

    freeMatrix(matA, N);
    freeMatrix(matArev, N);

    return 0;
}

