#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "bitmap.h"

Bitmap* newBitmap(void) {
	Bitmap *b = (Bitmap*)malloc(sizeof(Bitmap));
	assert(b);
	b->rd_pos = 0;
	memset(b, 0, sizeof(Bitmap));
	return b;
}

void freeBitmap(Bitmap *b) {
	if (!b)
		return;
	Bitmap *n = b->next;
	free(b);
	freeBitmap(n);
}

u64 bitmapSize(Bitmap *b) {
	u64 sz = 0;
	while (b) {
		sz += b->bits_used;
		b = b->next;
	}
	return sz;
}

inline u8 setBit(u8 a, u8 bit_no, u8 val) {
	return (val ? a | (1 << bit_no) : a & ~(1 << bit_no));
}

inline u8 getBit(u8 a, u8 bit_no) {
	return (a >> bit_no) & 1;
}

Bitmap* writeBit(u8 bit, Bitmap *b) {
	while (b->next) 
		b = b->next;

	bit &= 1;

	u64 bits_used = b->bits_used;
	if (bits_used + 1 <= BUFFER_SIZE) {
		if (bits_used % 8 == 0) {
			b->buf[bits_used / 8] = bit;
		} else {
			b->buf[bits_used / 8] = setBit(b->buf[bits_used / 8], bits_used & 7, bit);
		}
		(b->bits_used)++;
	} else {
		Bitmap *b2 = newBitmap();
		b->next = b2;
		b2->buf[0] = bit;
		(b2->bits_used)++;
	}

	return b;
}

// пока очевидная и долгая реализация
Bitmap* writeByte(u8 byte, Bitmap *b) {
	int i;
	for (i = 0; i < 8; ++i)
		writeBit((byte >> i), b);
	return b;
}

void printBitmap(Bitmap *b, FILE *out) {
	do {
		int i;
		for (i = 0; i < b->bits_used / 8 + (b->bits_used & 0xff ? 1 : 0); ++i) {
			putc(b->buf[i], out);
		}

		b = b->next;
	} while (b);
}

void resetReads(Bitmap *b) {
	if (!b)
		return;
	b->rd_pos = 0;
	resetReads(b->next);
}

u8 readBit(Bitmap *b, int *error) {
	while (b->next && b->rd_pos == BUFFER_SIZE) 
		b = b->next;
	
	// now we're in "active" reading part of list

	*error = 0;
	if (b->rd_pos < b->bits_used) {		
		int rd_pos = b->rd_pos;
		(b->rd_pos)++;
		
		if (rd_pos % 8 == 0 ) 
			return b->buf[rd_pos / 8] & 1;
		else 
			return getBit(b->buf[rd_pos / 8], rd_pos & 7);
	} else {
		*error = 1;
		return 0; // all bitmap read
	}
}

// пока очевидная и долгая реализация
u8 readByte(Bitmap *b, int *error) {
	int i;
	u8 result = 0;
	for (i = 0; i < 8; ++i) {
		u8 read = readBit(b, error);
		if (*error)
			return 0;
		result |= read << i;
	}

	return result;
}

inline int min(int a, int b) {
	return (a < b ? a : b);
}

void sprintBitmap(Bitmap *b) {
	int total_bits = 0;
	while (1) {
		int i, j;
		int to = min(b->bits_used / 8 + (b->bits_used % 8 == 0 ? 0 : 1), BUFFER_SZ_BYTES);
		for (i = 0; i < to; ++i) {
			for (j = 0; j < min(8, b->bits_used - i * 8); ++j) {
				printf("%d", (b->buf[i] >> j) & 1);
			}
			//printf(" ");
		}
		total_bits += b->bits_used;
		if (!b->next)
			break;
		else
			b = b->next;
		printf(" * ");
	}
	printf("\nTotal bits used: %d\n", total_bits);
}
