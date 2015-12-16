#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

typedef enum {false, true} bool;

void allocMatrix(float ***mat, int n, bool fillZeros) {
    int i, j;
    *mat = (float**)malloc(sizeof(float*) * n);
    for (i = 0; i < n; ++i) {
        (*mat)[i] = (float*)malloc(sizeof(float) * n);
        if (fillZeros)
            memset((*mat)[i], 0, sizeof(float) * n);
    }
}

void freeMatrix(float **mat, int n) {
    int i;
    for (i = 0; i < n; ++i)
        free(mat[i]);
    free(mat);
}

void genRandMatrix(float ***mat, int n) {
    allocMatrix(mat, n, true);
    int i, j;
    for (i = 0; i < n; ++i) 
        for (j = i; j < n; ++j)
            (*mat)[i][j] = rand() % 10;
}

void genIdMatrix(float ***mat, int n) {
    allocMatrix(mat, n, true);
    int i;
    for (i = 0; i < n; ++i)
        (*mat)[i][i] = 1;
}

void copyMatrix(float **dst, float **src, int n) {
    int i;
    for (i = 0; i < n; ++i)
        memcpy(dst[i], src[i], sizeof(float) * n);
}

void transposeMatrix(float **matIn, float **matOut, int n) {
    int i, j;

    if (matIn != matOut) {
        for (i = 0; i < n; ++i) {
            for (j = 0; j < n; ++j) {
                matOut[i][j] = matIn[j][i];
            }
        }
    } else {
        float tmp;
        for (i = 0; i < n; ++i) {
            for (j = i + 1; j < n; ++j) {
                tmp = matIn[i][j];
                matIn[i][j] = matIn[j][i];
                matIn[j][i] = tmp;
            }
        }
    }
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

float mulVectors(float *va, float *vb,  int n) {
    float res = 0.0f;
    int i;
    for (i = 0; i < n; ++i) 
        res += va[i] * vb[i];
    
    return res;
}

void mulMatrixTransposed2(float **matA, float **matBtrans, float **matOut, int n) {
    int i, j;
    for (i = 0; i < n; ++i) 
        for (j = 0; j < n; ++j)
            matOut[i][j] = mulVectors(matA[i], matBtrans[j], n);
}


void linCombMatrix(float c1, float **matA, float c2, float **matB, float **matOut, int n) {
    if (!matA && !matB)
        return;
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            matOut[i][j] = (matA ? matA[i][j] : 0) * c1 +
                           (matB ? matB[i][j] : 0) * c2;
        }
    }
}

void mulScalar(float **matIn, float **matOut, int n, float scalar) {
    linCombMatrix(scalar, matIn, 0.0f, NULL, matOut, n);
}

void matrixAdd(float **matA, float **matB, float **matOut, int n) {
    linCombMatrix(1.0f, matA, 1.0f, matB, matOut, n);
}

void matrixSub(float **matA, float **matB, float **matOut, int n) {
    linCombMatrix(1.0f, matA, -1.0f, matB, matOut, n);
}

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

float calcMaxSum(float **mat, int n, bool col) {
    float maxSum = -1.0f;
    float sum;
    int i, j;
    for (j = 0; j < n; ++j) {
        sum = 0.0f;
        for (i = 0; i < n; ++i) 
            sum += fabs(col ? mat[i][j] : mat[j][i]);
        maxSum = max(maxSum, sum);
    }

    return maxSum;
}

// ||A||1
float calcMaxColSum(float **mat, int n) {
    return calcMaxSum(mat, n, true);
}

// ||A||infty
float calcMacRowSum(float **mat, int n) {
    return calcMaxSum(mat, n, false);
}

void printMatrix(float **mat, int n);

// m -- iteration count
void matrixReverse(float **matA, float **matArev, int N, int M) {
    float  **matB, **matR, **matI;
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
    mulMatrix(matB, matA, matR, N);
    linCombMatrix(-1.0, matR, 1.0, matI, matR, N);
    // R done.
    
    float **matTmp;
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

// m -- iteration count
/*void matrixReverse(float **matA, float **matArev, int N, int M) {
    float  **matB, **matR, **matI;
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
    mulMatrix(matB, matA, matR, N);
    linCombMatrix(-1.0, matR, 1.0, matI, matR, N);
    // R done.
    
    float **matTmp;
    allocMatrix(&matTmp, N, false);
    int i;
    for (i = 0; i < M; ++i) {
        matrixAdd(matI, matArev, matArev, N);
        mulMatrix(matI, matR, matTmp, N);
        copyMatrix(matI, matTmp, N);
        printf("%d/%d done\n", (i + 1), M);
    }
    mulMatrix(matArev, matB, matTmp, N);
    copyMatrix(matArev, matTmp, N);

    freeMatrix(matB, N);
    freeMatrix(matR, N);
    freeMatrix(matI, N);
}*/

void printMatrix(float **mat, int n) {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) {
            printf("%f\t", mat[i][j]);
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
    float **matA, **matArev;
    genRandMatrix(&matA, N);
//    printf("Matrix A: \n");
//    printMatrix(matA, N);

    allocMatrix(&matArev, N, true);

    matrixReverse(matA, matArev, N, M);

 //   printf("Reverse A: \n");
 //   printMatrix(matArev, N);

    freeMatrix(matA, N);
    freeMatrix(matArev, N);

    return 0;
}

