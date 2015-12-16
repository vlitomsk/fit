#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <float.h>
#include <math.h>

/*
На P6100 размеры L1, L2 кратны размеру L3, поэтому будем работать со 
смещением, равным размеру L3, чтобы "забивать" банки одного и того же множества
*/
#define L3_CACHE_SIZE (3*1024*1024)
#define OFFSET L3_CACHE_SIZE

#define MAX_FRAGMENTS 32
#define PASS_COUNT 10
#define BLOCK_SIZE (MAX_FRAGMENTS * L3_CACHE_SIZE)

double traverse(const size_t *arena, size_t arrSize) {
  union ticks {
    uint64_t t64;
    struct s32 { uint32_t th, tl; } t32;
  } start, end;

  volatile size_t k, i, pass;
  double minCycles = DBL_MAX;
  for (k = 0, i = 0; i < arrSize; ++i) // heating 
    k = arena[k];
  for (pass = 0; pass < PASS_COUNT; ++pass) {
    asm("rdtsc\n" : "=a" (start.t32.th), "=d" (start.t32.tl));
    for (k = 0, i = 0; i < arrSize; ++i) 
      k = arena[k];
    asm("rdtsc\n" : "=a" (end.t32.th), "=d" (end.t32.tl));
    minCycles = fmin(minCycles, (end.t64 - start.t64) / (double)arrSize);
  }

  return minCycles;
}

int main(int argc, char **argv) {
  FILE *logfile = stdout;
  if (argc >= 2) {
    printf("Using %s as output\n", argv[1]);
    if (!(logfile = fopen(argv[1], "w"))) {
      perror("Can't open output file");
      return EXIT_FAILURE;
    }
  }

  size_t *arena = (size_t*)malloc(OFFSET * MAX_FRAGMENTS * sizeof(size_t));
  if (!arena) {
    perror("Can't allocate arena");
    fclose(logfile);
    return EXIT_FAILURE;
  }

  size_t fragments, i, j;
  for (fragments = 1; fragments <= MAX_FRAGMENTS; ++fragments) {
    for (i = 0; i < fragments; ++i) {
      size_t subblockSize = BLOCK_SIZE / fragments;
      for (j = 0; j < subblockSize; ++j) 
        arena[i * OFFSET + j] = ((i + 1) % fragments) * OFFSET + 
                                ((i == fragments - 1) ? (j + 1) % subblockSize : j);
    }

    double minCycles = traverse(arena, BLOCK_SIZE);
    fprintf(logfile, "%zu %lf\n", fragments, minCycles);
    fflush(logfile);
  }
  fclose(logfile);
  free(arena);

  return EXIT_SUCCESS;
}

