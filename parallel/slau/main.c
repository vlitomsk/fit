#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <openmpi/mpi.h>
#include "inpDesc.h"
#include "common.h"
#include "taskParams.h"

const int rootNode = 0;

double scalarMul(taskParams *p, const double *a, const double *b, int rootNode) {
  int i;
  double sum, nodeSum = 0.0;
  MPI_Status st;

  for (i = p->startY; i < p->startY + p->blockDimY; ++i)
    nodeSum += a[i] * b[i];

  MPI_Reduce(&nodeSum, &sum, 1, MPI_DOUBLE, MPI_SUM, rootNode, MPI_COMM_WORLD);
  return sum;
}

void sumVec(taskParams *p, double acoef, const double *a, double bcoef, const double *b, double *out) {
  int i;
  for (i = 0; i < p->matSize; ++i)
    out[i] = acoef * a[i] + bcoef * b[i];
}

/* calculates eucl.norm of vector a splitted between nodes (according to taskParams), then
   sqrt $ foldl (+) 0 partialSums */
double euclNorm(taskParams *p, const double *a, int rootNode) {
  return sqrt(scalarMul(p, a, a, rootNode));
}

void mulMatVec(taskParams *p, matDesc *matd, double *vec, double *result) {
  int i, j;
  int *displs, *recvCounts = NULL;
  if (p->commRank >= p->effSize)
    return;

  memset(result, 0, p->matSize * sizeof(double));
  for (p->roundNo = 0; p->roundNo < p->effSize; ++(p->roundNo)) {
    updParams(p); // recalc dimX, startX
    readSubmat(matd, p); // cached; no barrier needed

    for (i = 0; i < p->blockDimY; ++i) {
      for (j = 0; j < p->blockDimX; ++j) {
        result[i + p->startY] += (vec + p->startX)[j] *
                                     (p->psubmat + p->matSize * i)[j];
      }
    }
    int src = p->commRank == 0 ? p->effSize - 1 : p->commRank - 1;
    int dst = (p->commRank + 1) % p->effSize;
    int src_offt = calcStartX(src, p->effSize, p->matSize);
    int src_sz = calcStartX(src + 1, p->effSize, p->matSize) - calcStartX(src, p->effSize, p->matSize);
  
    MPI_Status st;
    MPI_Sendrecv(vec + p->startX, p->blockDimX, MPI_DOUBLE, 
                 dst, 0, vec + src_offt, src_sz, MPI_DOUBLE,
                 src, 0, MPI_COMM_WORLD, &st);
  }
}

bool solutionOk(double eps, taskParams *p, matDesc *A, const double *b, const double *y, int rootNode) {
  static bool bNormCalculated = false;
  static double bNorm, nrm;

  if (!bNormCalculated) {
    bNorm = euclNorm(p, b, rootNode);
    bNormCalculated = true;
  }

  nrm = euclNorm(p, y, rootNode);
  
  if (p->commRank == rootNode) {
    return nrm / bNorm < eps;
  }
  return false;
}

/**
 * Solving Ax = b system iteratively : 
 *   y(n) = A * x(n) - b
 *   tau(n) = (y(n) . A * y(n)) / (A * y(n) . A * y(n))
 *   x(n + 1) = x(n) - tau(n) * y(n)
 *   x(0) = random vector
 *
 * Solution found when :
 *   ||A * x(n) - b|| / ||b|| < eps
 */
bool solve(int maxIters, double eps, matDesc *A, double **result, int *matSize, int rootNode, taskParams *tp) {
  int i, j;
  bool ext = false, solFound = false;
  double *y = NULL, *Ay, tau, *res, *b;

  *matSize = tp->matSize;
  b = tp->pvec;
  res = *result = (double*)malloc(3 * tp->matSize * sizeof(double));
  if (!res) {
    perror("Can't alloc memory");
    return false;
  }
  y = res + tp->matSize;
  Ay = res + 2 * tp->matSize; 
  
  if (tp->commRank == rootNode) { 
    for (i = 0; i < tp->matSize; ++i) 
      res[i] = (rand() % 10);
/*    res[0] = -1.920;
    res[1] = -9.163;
    res[2] = -7.871;
    res[3] = -6.841;*/
//    memset(res, 0, tp->matSize * sizeof(double));
  }
  
  MPI_Bcast(res, tp->matSize, MPI_DOUBLE, rootNode, MPI_COMM_WORLD);

  for (i = 0; !solFound && i < maxIters; ++i) {
    mulMatVec(tp, A, res, y); 
    sumVec(tp, 1.0, y, -1.0, b, y);
    mulMatVec(tp, A, y, Ay); 
    double tauNum = scalarMul(tp, y, Ay, rootNode); 
    double tauDenom = scalarMul(tp, Ay, Ay, rootNode); 
    
    tau = tauNum / tauDenom;
    MPI_Bcast(&tau, 1, MPI_DOUBLE, rootNode, MPI_COMM_WORLD);
    sumVec(tp, 1.0, res, -tau, y, res);
    solFound = solutionOk(eps, tp, A, tp->pvec, y, rootNode);
    int solFoundI = (int)solFound;
    MPI_Bcast(&solFoundI, 1, MPI_INT, rootNode, MPI_COMM_WORLD);
    solFound = solFoundI;
  }

  fprintf(stderr, "ITERATIONS: %d\n", i);

  *result = (double*)realloc(*result, tp->matSize * sizeof(double)); 
  return solFound;
}

int initParams(taskParams *tp) {
  MPI_Comm_rank(MPI_COMM_WORLD, &tp->commRank);
  MPI_Comm_size(MPI_COMM_WORLD, &tp->commSize);

  return 0;
}

int main(int argc, char **argv) {
  int commRank, matSize, i, commSize;
  bool success;
  matDesc md;

/*  if (argc != 2) {
    fprintf(stderr, "Usage: %s matFile\n", argv[0]);
    return EXIT_FAILURE;
  }*/

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);

  taskParams tp;
  initParams(&tp);
  double t1, t2, t3, t4;
  t1 = MPI_Wtime();
  for (i = 0; i < commSize; ++i) {
    if (commRank == i) {
      t3 = MPI_Wtime();
      //md = openFileDesc(argv[1], &success, &tp);
      md = openMemDesc(&success, &tp);
      t4 = MPI_Wtime();
      fprintf(stderr, "[%d] openFileDesc time: %lf\n", commRank, (t4 - t3));
      if (!success) {
        fprintf(stderr, "Can't open file desc\n");
        MPI_Finalize();
        return EXIT_FAILURE;
      }
    }
    MPI_Barrier(MPI_COMM_WORLD);
  }
  t2 = MPI_Wtime();
  fprintf(stderr, "[%d] total Reading time: %lf\n", commRank, (t2 - t1));

  double *res; 
  t1 = MPI_Wtime();
  bool solved = solve(200, 1e-9, &md, &res, &matSize, rootNode, &tp);
  t2 = MPI_Wtime();
  fprintf(stderr, "[%d] solving time: %lf\n", commRank, t2 - t1);
  if (solved) {
    if (commRank == rootNode) {
      puts("Found solution");
      fputs("Found solution\n", stderr);
      for (i = 0; i < matSize; ++i)
        printf("%lf ", res[i]);
      puts(""); 
    }
  } else if (commRank == rootNode) {
    puts("No solution found");
  }
  free(res);
  //closeFileDesc(&md);
  closeMemDesc(&md);
  MPI_Finalize();

  return EXIT_SUCCESS;
}

