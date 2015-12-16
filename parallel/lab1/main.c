#include <stdio.h>
#include <stdlib.h>
#include <openmpi/mpi.h>

#define _LOGGING_

static int __commRank;
#ifdef _LOGGING_
#define log(...) { printf("[%d] ", __commRank); printf(__VA_ARGS__); }
#else
#define log(...) 
#endif

typedef enum {false = 0, true = 1} bool;
typedef enum {TAG_INPUTACCESS} MPITag_t;

typedef struct {
  int i, matSize, blockDimX, blockDimY, startRow, startCol, roundNo, commRank, commSize;
  double *pvec, *presult, *psubmat;
} taskParams;

int allocVecMat(taskParams *p, int submatMaxSize) {
  p->pvec = (double*)malloc((p->matSize * 2 + submatMaxSize) * sizeof(double));
  if (!p->pvec) {
    perror("Can't alloc memory");
    return 1;
  }
  p->presult = p->pvec + p->matSize;
  p->psubmat = p->pvec + 2 * p->matSize;

  return 0;
}

int calcStartRow(int commRank, int commSize, int matSize) {
  int rest = matSize % commSize;
  return  commRank < rest ? (matSize / commSize + 1) * commRank
                          : rest - 1 + commRank * matSize / commSize;
}

void calcParams(taskParams *p) {
  int commRank = p->commRank, 
      commSize = p->commSize, 
      matSize = p->matSize;
  p->startRow = calcStartRow(commRank, commSize, matSize);
  int offset = (commRank + p->roundNo) % commSize;
  p->startCol = calcStartRow(offset, commSize, matSize);
  p->blockDimX = calcStartRow(offset + 1, commSize, matSize) - p->startCol;
  p->blockDimY = calcStartRow(commRank + 1, commSize, matSize) - p->startRow;
  
  log("roundNo = %d; blockDim = (%d, %d); start(Row,Col) = (%d,%d)\n", 
      p->roundNo, p->blockDimX, p->blockDimY, p->startRow, p->startCol);
}

void passNewLine(FILE *fd) {
  char ch;
  while ((ch = fgetc(fd)) != '\n') {}
}

int readRoundInput(FILE *fd, const taskParams *p) {
  MPI_Status st;
  int i, j, tmp;

  static bool readOnce = false; /* changes to TRUE after reading */

  if (!readOnce) {
    for (i = 0; i < p->matSize; ++i) /* reading vector */
      fscanf(fd, "%lf", p->pvec + i);

    for (i = 0; i < p->startRow + 1; ++i) /* passing matrix rows */
      passNewLine(fd);
  }

  static int matFilePos = ftell(fd);
  if (readOnce)
    fseek(fd, matFilePos, SEEK_SET);
  for (i = 0; i < p->blockDimY; ++i)
    for (j = 0; j < p->matSize; ++j)
      if (j >= p->startCol && j < p->startCol + p->blockDimX)
        fscanf(fd, "%lf", p->psubmat + i * p->blockDimY + j - p->startCol);
      else
        fscanf(fd, "%lf", &tmp); /* pull number from stream */

  readOnce = true;

  /*log("COLS: ");
  for (i = 0; i < matSize * matSize; ++i)
    log("%lf ", psubmat[i]);
  log("\n");*/

  return 0;
}

double scalarMul(const double *pvecA, const double *pvecB, int N) {
  int i;
  double sum = 0.0;
  for (i = 0; i < N; ++i) 
    sum += pvecA[i] * pvecB[i];
  return sum;
}

void partMul(taskParams *p) {
  int i;
  for (i = p->startRow; i < p->blockDimY; ++i) 
    p->presult[i] = scalarMul(p->pvec + p->startCol, p->psubmat + p->blockDimY * i, p->blockDimX);
}

#define sqr(x) ((x)*(x))

int run(const char *inpPath) {
  int roundNo, i;
  taskParams p;
  FILE *fd;

  MPI_Comm_size(MPI_COMM_WORLD, &(p.commSize));
  MPI_Comm_rank(MPI_COMM_WORLD, &(p.commRank));
  __commRank = p.commRank;

  log("Hi! (rank, size) = (%d, %d)\n", p.commRank, p.commSize);
  fd = fopen(inpPath, "r");
  if (!fd) {
    perror("Can't open input file");
    return 1;
  }

  fscanf(fd, "%d", &(p.matSize));
  if (allocVecMat(&p, sqr(p.matSize / p.commSize + 1))) {
    fclose(fd);
    return 1;
  }
  
  for (roundNo = 0; roundNo < p.commSize; ++roundNo) {
    log("round %d reading ...\n", roundNo);
    for (i = 0; i < p.commSize; ++i) {
      if (i == p.commRank) {
        if (readRoundInput(fd, roundNo)) {
          fclose(fd);
          free(p.pvec);
          return 1;
        }
      }
      MPI_Barrier(MPI_COMM_WORLD);
    }
    calcParams(&p);
    partMul(&p);
    log("round %d done; waiting for other processes ...\n", roundNo);
    MPI_Barrier(MPI_COMM_WORLD); // TODO is it necessary?
    
    /* TODO Implement gathering */
  }
  
/*  log("collecting results\n");
  int i, *displs, *recvCounts = (int*)malloc(2 * commSize * sizeof(int));
  if (!recvCounts) {
    perror("Can't alloc memory");
    fclose(fd);
    free(pvec);
    return 1;
  }
  displs = recvCounts + commSize;
  for (i = 0; i < commSize; ++i) {
    recvCounts[i] = matSize / commSize + (i < matSize % commSize ? 1 : 0);
    displs[i] = i ? displs[i - 1] + recvCounts[i - 1]: 0;
  }
  MPI_Gatherv(presult + startRow, blockDimY, MPI_DOUBLE,
              presult, recvCounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);

  if (commRank == 0) {
    log("done gathering\n");
  
    log("Result: \n");
    for (i = 0; i < matSize; ++i)
      log("%lf ", presult[i]);
    log("\n");
  }
  free(recvCounts);*/

  fclose(fd);
  free(pvec);

  return 0;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s matFile\n", argv[0]);
    return 1;
  }

  MPI_Init(&argc, &argv);
  int ecode = run(argv[1]);
  MPI_Finalize();

  return ecode;
}

