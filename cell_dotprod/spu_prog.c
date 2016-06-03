#include <stdint.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>
#include "consts.h"

typedef union {
	unsigned int i;
	float f;
} float_uint_t;

#define BUF_SZ_BYTES (2 * CHUNK_SZ_BYTES)
#define BUF_SZ_FLOATS (BUF_SZ_BYTES / sizeof(float))
#define buf_ptr_float(buffers, buf_idx, vec_idx) (buffers + buf_idx * BUF_SZ_FLOATS + vec_idx * CHUNK_SZ_FLOATS)
#define buf_ptr_vecfloat(buffers, buf_idx, vec_idx) ((vector float *) (buf_ptr_float(buffers, buf_idx, vec_idx)))

#define MAXITER (SUBVEC_SZ_CHUNKS / NBUFFERS)

int 
main(
	unsigned long long spe_id, 
	unsigned long long ppu_vector_a, 
	unsigned long long ppu_vector_b)
{
	int i, iter, buf_idx, vec_idx;
	unsigned long long ppu_vector_bases[2] _ALIG(128);
	vector float * pchunk_a, * pchunk_b;
	vector float g_vec = {0,0,0,0};

	ppu_vector_bases[0] = ppu_vector_a;
	ppu_vector_bases[1] = ppu_vector_b;

	const unsigned int spu_num = spu_read_in_mbox();
	unsigned long long get_edge_bytes = spu_num * SUBVEC_SZ_BYTES;

	float buffers[NBUFFERS * BUF_SZ_FLOATS] _ALIG(128);
	int buffer_tags[NBUFFERS][2] _ALIG(128);
	//int buffer_tags[NBUFFERS];

	for (iter = 0; iter < NBUFFERS; ++iter) {
		buffer_tags[iter][0] = mfc_tag_reserve();
		buffer_tags[iter][1] = mfc_tag_reserve();
	}
	
	// first mfc_get for all
	for (buf_idx = 0; buf_idx < NBUFFERS; ++buf_idx) {
		for (vec_idx = 0; vec_idx < 2; ++vec_idx) {
			mfc_get(buf_ptr_float(buffers, buf_idx, vec_idx), 
					ppu_vector_bases[vec_idx] + get_edge_bytes, 
					CHUNK_SZ_BYTES, 
					buffer_tags[buf_idx][vec_idx], 
					0, 0);
		}
	}
	get_edge_bytes += CHUNK_SZ_BYTES;

	//printf("subvec_sz-chunks: %d\n", SUBVEC_SZ_CHUNKS);
	//printf("%d==%d\n", MAXITER*NBUFFERS*CHUNK_SZ_FLOATS, SUBVEC_SZ_FLOATS);
	int chunksleft = SUBVEC_SZ_CHUNKS;
	while(chunksleft!=0) {
		for (buf_idx = 0; chunksleft !=0 && buf_idx < NBUFFERS; ++buf_idx) {
			const int tag_mask = (1 << buffer_tags[buf_idx][0])
								| (1 << buffer_tags[buf_idx][1]);
				
			mfc_write_tag_mask(tag_mask);
			mfc_read_tag_status_all();

			pchunk_a = buf_ptr_vecfloat(buffers, buf_idx, 0);
			pchunk_b = buf_ptr_vecfloat(buffers, buf_idx, 1);

			for (i = 0; i < CHUNK_SZ_FLOATVECS; ++i) {
				g_vec = spu_madd(pchunk_a[i], pchunk_b[i], g_vec);
			}

			// move this mfc_get to end of loop, check get_edge_bytes variable dynamics
			if (likely(iter != MAXITER - 1)) {
				for (vec_idx = 0; vec_idx < 2; ++vec_idx) {
					mfc_get(buf_ptr_float(buffers, buf_idx, vec_idx), 
							ppu_vector_bases[vec_idx] + get_edge_bytes, 
							CHUNK_SZ_BYTES, 
							buffer_tags[buf_idx][vec_idx], 
							0, 0);
				}
			}
			get_edge_bytes += CHUNK_SZ_BYTES;
			--chunksleft;
		}
	}

	for (iter = 0; iter < NBUFFERS; ++iter) {
		mfc_tag_release(buffer_tags[iter][0]);      
		mfc_tag_release(buffer_tags[iter][1]);      
	}

	float_uint_t retval;
	retval.f = 
		spu_extract(g_vec, 0) +
		spu_extract(g_vec, 1) +
		spu_extract(g_vec, 2) +
		spu_extract(g_vec, 3);

	//printf("retval: %f\n", retval.f);
	spu_write_out_mbox(retval.i);

	return 0;
}
