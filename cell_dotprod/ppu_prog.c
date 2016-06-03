#include <libspe2.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "consts.h"
#include <time.h>
#include <assert.h>

extern spe_program_handle_t SPU_SYMB;

static float vec_a[IN_VEC_SZ_FLOATS] _ALIG(128);
static float vec_b[IN_VEC_SZ_FLOATS] _ALIG(128);
spe_context_ptr_t context[THREAD_COUNT];
unsigned int ids[THREAD_COUNT];

void *
thread_routine(
    void * arg)
{
  int spu_idx = *((int *) arg);
  spe_context_ptr_t ctx = context[spu_idx];
  unsigned int entry = SPE_DEFAULT_ENTRY;
  //printf("PPU starting context %d\n", spu_idx);
  spe_context_run(ctx, &entry, 0, vec_a, vec_b, NULL);
}

typedef union {
  float f;
  unsigned int i;
} flint_union;

void
fill_vectors()
{
  int i;
  for (i = 0; i < IN_VEC_SZ_FLOATS; ++i) {
    vec_a[i] = 0.5f;
    vec_b[i] = (i % 2) ? 2.0f : 2.0f;
	//printf("%f ", vec_b[i]);
  }
}

int
main ()
{
  int i, err;
  pthread_t pthreads[THREAD_COUNT];

  fill_vectors();

  for (i = 0; i < THREAD_COUNT; ++i) {
    ids[i] = i;
    context[i] = spe_context_create(0, NULL);
    spe_program_load(context[i], &SPU_SYMB);
  }
  
  struct timespec t_start, t_end;
  clock_gettime(CLOCK_MONOTONIC, &t_start);
  for (i = 0; i < THREAD_COUNT; ++i) {
    if (err = pthread_create(pthreads + i, NULL, thread_routine, ids + i)) {
      fprintf(stderr, "pthread_create() : %s\n", strerror(err)); 
      return EXIT_FAILURE;
    }
    spe_in_mbox_write(context[i], ids + i, 1, SPE_MBOX_ANY_NONBLOCKING);
  }

  float result = 0.0;
  for (i = SUBVEC_SZ_FLOATS * THREAD_COUNT; i < IN_VEC_SZ_FLOATS; ++i) {
	result += vec_a[i] * vec_b[i];
  }
  //assert(floor(result)==REST_SZ_FLOATS);
  //printf("pre result: %f\n", result);

  for (i = 0; i < THREAD_COUNT; ++i) {
    flint_union un;
    while (spe_out_mbox_status(context[i]) == 0) {}
    spe_out_mbox_read(context[i], &un.i, 1);
    result += un.f;
	//assert(floor(un.f)==SUBVEC_SZ_FLOATS);
	//printf("partial: %f\n", un.f);
  }
  clock_gettime(CLOCK_MONOTONIC, &t_end);

  printf("time : %lf ms\n", 1e-6 * (((double)t_end.tv_sec * 1e9 + t_end.tv_nsec) - ((double)t_start.tv_sec * 1e9 + t_start.tv_nsec)));

  for (i = 0; i < THREAD_COUNT; ++i) {
    spe_context_destroy(context[i]);
    if (err = pthread_join(pthreads[i], NULL)) {
      fprintf(stderr, "pthread_join() : %s\n", strerror(err)); 
    }
  }

  printf("Result : %f\n", result);
  assert(floor(result) == 8388608);

  return EXIT_SUCCESS;
}
