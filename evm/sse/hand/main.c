#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef enum {false, true} bool;

// size of matrix must be aligned to 16-byte (128-bit) (4 floats)

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

typedef float v4sf __attribute__ ((vector_size (16)));
typedef union {
    v4sf v;
    float a[4];
} Vec4;

float mulVectors(float *va, float *vb,  int n) {
    int i;

    Vec4 p;
    p.v = (v4sf){0.0f, 0.0f, 0.0f, 0.0f};
    v4sf *v1 = (v4sf*)va;
    v4sf *v2 = (v4sf*)vb;
    for (i = 0; i < (n >> 2); ++i) 
        p.v += v1[i] * v2[i];
    
    float result = p.a[0] + p.a[1] + p.a[2] + p.a[3];
    return result;
}

void mulMatrix(float **matA, float **matB, float **matOut, int n) {
    int i, j, k;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            matOut[i][j] = 0.0f;
            for (k = 0; k < n; ++k) {
                matOut[i][j] += matA[i][k] * matB[k][j];
            }
        }
    }
}
        
void mulMatrixTransposed2(float *matA, float *matBtrans, float *matOut, int n) {
    int i, j;
    for (i = 0; i < n; ++i) 
        for (j = 0; j < n; ++j)
            matOut[i * n + j] = mulVectors(matA + i * n, matBtrans + j * n, n);
}

void linCombMatrix(float c1, float *matA, float c2, float *matB, float *matOut, int n) {
    int i, j;
   
    v4sf vcoef1 = (v4sf){c1, c1, c1, c1};
    v4sf vcoef2 = (v4sf){c2, c2, c2, c2};
    v4sf zeros = (v4sf){0.0f, 0.0f, 0.0f, 0.0f};

    for (i = 0; i < n; ++i) {
        v4sf *v1 = matA ? (v4sf*)(matA + i * n) : NULL;
        v4sf *v2 = matB ? (v4sf*)(matB + i * n) : NULL;
        v4sf *vout = (v4sf*)(matOut + i * n);
        for (j = 0; j < (n >> 2); ++j) {
            vout[j] = (v1 ? v1[j] : zeros) * vcoef1 +
                         (v2 ? v2[j] : zeros) * vcoef2;
        }
    }
}

void mulScalar(float *matIn, float *matOut, int n, float scalar) {
    linCombMatrix(scalar, matIn, 0.0f, NULL, matOut, n);
}

void matrixAdd(float *matA, float *matB, float *matOut, int n) {
    linCombMatrix(1.0f, matA, 1.0f, matB, matOut, n);
}

void matrixSub(float *matA, float *matB, float *matOut, int n) {
    linCombMatrix(1.0f, matA, -1.0f, matB, matOut, n);
}

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

float calcMaxSum(float *mat, int n, bool col) {
    float maxSum = -1.0f;
    float sum;
    int i, j;
    for (j = 0; j < n; ++j) {
        sum = 0.0f;
        for (i = 0; i < n; ++i) 
            sum += fabs(col ? mat[i * n + j] : mat[j * n + i]);
        maxSum = max(maxSum, sum);
    }

    return maxSum;
}

// ||A||1
float calcMaxColSum(float *mat, int n) {
    return calcMaxSum(mat, n, true);
}

// ||A||infty
float calcMacRowSum(float *mat, int n) {
    return calcMaxSum(mat, n, false);
}

void printMatrix(float *mat, int n);
// m -- iteration count
void matrixReverse(float *matA, float *matArev, int N, int M) {
    float  *matB, *matR, *matI;
    genIdMatrix(&matI, N);

    // Calculating B matrix...
    allocMatrix(&matB, N, false);
    transposeMatrix(matA, matB, N);
    float A_1 = calcMaxColSum(matA, N);
    float A_inf = calcMacRowSum(matA, N);
    mulScalar(matB, matB, N, 1.0f / A_1);
    mulScalar(matB, matB, N, 1.0f / A_inf);
    // B done.

    //Calculating R matrix..
    allocMatrix(&matR, N, false);
    transposeMatrix(matA, matA, N);
    mulMatrixTransposed2(matB, matA, matR, N);
    linCombMatrix(-1.0, matR, 1.0, matI, matR, N);
    // R done.
    
    float *matTmp;
    allocMatrix(&matTmp, N, false);
    int i;
    transposeMatrix(matR, matR, N);
    for (i = 0; i < M; ++i) {
        if (!(i & 1)) {
            matrixAdd(matI, matArev, matArev, N);
            mulMatrixTransposed2(matI, matR, matTmp, N);
        } else {
            matrixAdd(matTmp, matArev, matArev, N);
            mulMatrixTransposed2(matTmp, matR, matI, N);
        }
    }
    transposeMatrix(matB, matB, N);
    mulMatrixTransposed2(matArev, matB, matTmp, N);
    copyMatrix(matArev, matTmp, N);

    freeMatrix(matB, N);
    freeMatrix(matR, N);
    freeMatrix(matI, N);
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

