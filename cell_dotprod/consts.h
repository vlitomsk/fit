#ifndef _CONSTS_H_INCLUDED_
#define _CONSTS_H_INCLUDED_

/* task params */

#ifndef NBUFFERS
#define NBUFFERS 4
#endif

#ifndef THREAD_COUNT
#define THREAD_COUNT 8 
#endif

#define IN_VEC_SZ_BYTES (32*1024*1024)
//#define IN_VEC_SZ_BYTES (32*1024)

#define IN_VEC_SZ_FLOATS (IN_VEC_SZ_BYTES / sizeof(float))


/* and everything else.. */

#define CHUNK_SZ_BYTES (16*1024)
#define VEC_SZ_BYTES 16

//#define SUBVEC_SZ_BYTES (IN_VEC_SZ_BYTES / THREAD_COUNT)
#define SUBVEC_SZ_BYTES ((((IN_VEC_SZ_BYTES / THREAD_COUNT) & (~(CHUNK_SZ_BYTES - 1)))/NBUFFERS)*NBUFFERS)
#define REST_SZ_BYTES (IN_VEC_SZ_BYTES - SUBVEC_SZ_BYTES * THREAD_COUNT)
#define REST_SZ_FLOATS (REST_SZ_BYTES / sizeof(float))
#define SUBVEC_SZ_FLOATS (SUBVEC_SZ_BYTES / sizeof(float))
#define SUBVEC_SZ_CHUNKS (SUBVEC_SZ_BYTES / CHUNK_SZ_BYTES)

#define VEC_SZ_FLOATS (VEC_SZ_BYTES / sizeof(float))

#define CHUNK_SZ_FLOATS (CHUNK_SZ_BYTES / sizeof(float))
#define CHUNK_SZ_FLOATVECS (CHUNK_SZ_FLOATS / VEC_SZ_FLOATS)

#define _ALIG(x) __attribute__ ((aligned(x)))

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)

#endif
