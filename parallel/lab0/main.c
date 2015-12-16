#include <stdio.h>
#include <stdlib.h>
#include <openmpi/mpi.h>

static int commRank, commSize;
static int matSize, startCol = 0, colsCount;
static double *pvec = NULL, *pcols = NULL, *presult = NULL;
typedef enum {TAG_INPUTACCESS} MPITag_t;

int readInput(const char *filePath) {
  MPI_Status st;
  int i;
  char ch;
  if (commRank != 0) { // wait until previous node read the file
    MPI_Recv(&startCol, 1, MPI_INT, commRank - 1, TAG_INPUTACCESS, MPI_COMM_WORLD, &st);

    if (st.MPI_ERROR != MPI_SUCCESS) {
      fprintf(stderr, "[%d] Can't MPI_Recv from prev. node\n", commRank);
      return 1;
    }
  }

  FILE *fd = fopen(filePath, "r");
  if (!fd) {
    perror("Can't open input file");
    return 1;
  }
  fscanf(fd, "%d", &matSize);
  colsCount = matSize / commSize + (commRank < matSize % commSize ? 1 : 0);
  printf("[%d] colsCount: %d; startCol: %d\n", commRank, colsCount, startCol);
  pvec = (double*)malloc(matSize * (colsCount + 2) * sizeof(double));
  if (!pvec) {
    perror("Can't alloc memory");
    fclose(fd);
    return 1;
  }
  presult = pvec + matSize;
  pcols = pvec + 2 * matSize;
  for (i = 0; i < matSize; ++i)
    fscanf(fd, "%lf", pvec + i);
  for (i = 0; i <= startCol; ++i)
    while ((ch = fgetc(fd)) != '\n') {}
  for (i = 0; i < colsCount * matSize; ++i)
    fscanf(fd, "%lf", pcols + i);
  fclose(fd);
  printf("[%d] file read OK\n", commRank);

  printf("[%d] COLS: ", commRank);
  for (i = 0; i < colsCount * matSize; ++i)
    printf("%lf ", pcols[i]);
  puts("");


  // send access to input file to the next node
  if (commRank != commSize - 1) {
    int tmp = startCol + colsCount;
    if (MPI_Send(&tmp, 1, MPI_INT, commRank + 1, TAG_INPUTACCESS, MPI_COMM_WORLD) != MPI_SUCCESS) {
      free(pvec);
      fprintf(stderr, "[%d] Can't MPI_Send to next node\n", commRank);
      return 1;
    }
  }

  return 0;
}

double scalarMul(const double *vecA, const double *vecB) {
  int i;
  double sum = 0.0;
  for (i = 0; i < matSize; ++i) 
    sum += vecA[i] * vecB[i];
  return sum;
}

void partMul(void) {
  int i;
  for (i = 0; i < colsCount; ++i) 
    presult[startCol + i] = scalarMul(pvec, pcols + matSize * i);
  printf("[%d] partMul done\n", commRank);
}

int run(const char *inpPath) {
  printf("[%d] Hello! (rank, size) = (%d, %d)\n", commRank, commRank, commSize);
  if (readInput(inpPath))
    return 1;

  partMul();
  printf("[%d] waiting for other processes\n", commRank);
  MPI_Barrier(MPI_COMM_WORLD);
  
  printf("[%d] collecting results\n", commRank);
  int i, *displs, *recvCounts = (int*)malloc(2 * commSize * sizeof(int));
  if (!recvCounts) {
    perror("Can't alloc memory");
    free(pvec);
    return 1;
  }
  displs = recvCounts + commSize;
  for (i = 0; i < commSize; ++i) {
    recvCounts[i] = matSize / commSize + (i < matSize % commSize ? 1 : 0);
    displs[i] = i ? displs[i - 1] + recvCounts[i - 1]: 0;
  }
  MPI_Gatherv(presult + startCol, colsCount, MPI_DOUBLE, 
              presult, recvCounts, displs, MPI_DOUBLE, 0, MPI_COMM_WORLD);  
  MPI_Barrier(MPI_COMM_WORLD);

  if (commRank == 0) {
    printf("[%d] done gathering\n");
  
    puts("Result: ");
    for (i = 0; i < matSize; ++i)
      printf("%lf ", presult[i]);
    puts("");
  }

  free(recvCounts);
  free(pvec);
  return 0;
}

//#define LOGFILE
int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s matFile\n", argv[0]);
    return 1;
  }

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &commSize);
  MPI_Comm_rank(MPI_COMM_WORLD, &commRank);
#ifdef LOGFILE
  char str[256];
  sprintf(str, "%d.log", commRank);
  freopen(str, "w", stdout);
#endif
  int ecode = run(argv[1]);
  MPI_Finalize();
  printf("[%d] Bye!\n", commRank);
#ifdef LOGFILE
  fflush(stdout);
#endif

  return ecode;
}

