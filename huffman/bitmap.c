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

// РїРѕРєР° РѕС‡РµРІРёРґРЅР°СЏ Рё РґРѕР»РіР°СЏ СЂРµР°Р»РёР·Р°С†РёСЏ
Bitmap* writeByte(u8 byte, Bitmap *b) {
	int i;
	for (i = 0; i < 8; ++i)
		writeBit((byte >> i), b);
	return b;
}

Bitmap* writeDWord(u32 dword, Bitmap *b) {
	writeByte(dword & 0xff, b);
	writeByte((dword >> 8) & 0xff, b);
	writeByte((dword >> 16) & 0xff, b);
	writeByte((dword >> 24) & 0xff, b);

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

// РїРѕРєР° РѕС‡РµРІРёРґРЅР°СЏ Рё РґРѕР»РіР°СЏ СЂРµР°Р»РёР·Р°С†РёСЏ
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

u32 readDWord(Bitmap *b, int *error) {
	u32 dw = readByte(b, error);
	dw |= (u32)readByte(b, error) << 8;
	dw |= (u32)readByte(b, error) << 16;
	dw |= (u32)readByte(b, error) << 24;

	return dw;
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

BRWState* initBRWS(void) {
	BRWState *bws = calloc(1, sizeof(BRWState));
	return bws;
}

/* bws -- bit write state. tut mozhet hranitsa nezapisanniy ostatok */
/* pri pervom vizove funkcii bws = 0 */
void writeBitThr(u8 bit, BRWState *bws, FILE *out) {
	bws->x |= (bit & 1) << bws->len;
	if (bws->len == 7) {
		fputc(bws->x, out);
		bws->len = bws->x = 0;
	} else {
		(bws->len)++;
	}
}

void writeByteThr(u8 byte, BRWState *bws, FILE *out) {
/*	11111111 | 00000001
	                 ^^ len = 2   
	  byte         x

	 пишем последовательность : (ост. 6 младших бит byte) 01
	 bws.x = ост.2 старших бита*/
	 
	 fputc(((1 << bws->len) - 1) & bws->x | (byte << bws->len), out);
	 bws->x = byte >> (8 - bws->len);
}

void writeDWordThr(u32 dword, BRWState *bws, FILE *out) {
	writeByteThr(dword & 0xff, bws, out);
	writeByteThr((dword >> 8) & 0xff, bws, out);
	writeByteThr((dword >> 16) & 0xff, bws, out);
	writeByteThr((dword >> 24) & 0xff, bws, out);
}

void writeDDWordThr(u64 ddword, BRWState *bws, FILE *out) {
	writeDWordThr(ddword & 0xffffffff, bws, out);
	writeDWordThr((ddword >> 32) & 0xffffffff, bws, out);
}

// write byte from bws to out
// aligns next write to u8 size
void flushBitWriter(BRWState *bws, FILE *out) {
	fputc(bws->x, out);
	fflush(out);
	bws->x = bws->len = 0;
}

u8 readBitThr(BRWState *brs, FILE *in, int *ok) {
	*ok = 1;
	if (brs->len == 0) {
		*ok = fread(&(brs->x), sizeof(u8), 1, in);
		brs->len = 8;
	}
	u8 res = brs->x & 1;
	(brs->len)--;
	brs->x >>= 1;
	return res;
}

u8 readByteThr(BRWState *brs, FILE *in, int *ok) {
	u8 ch;
	*ok = fread(&ch, sizeof(u8), 1, in);
	if (brs->len != 0) {
		u8 res = brs->x & ((1 << brs->len) - 1) | (ch << brs->len);
		brs->x = ch >> (8 - brs->len);
		return res;
	} else
		return ch;
}

u32 readDWordThr(BRWState *brs, FILE *in, int *ok) {
	u32 a, b, c, d;
	a = readByteThr(brs, in, ok);	
	b = readByteThr(brs, in, ok);
	c = readByteThr(brs, in, ok);
	d = readByteThr(brs, in, ok);
	return a | (b << 8) | (c << 16) | (d << 24);
}

u64 readDDWordThr(BRWState *brs, FILE *in, int *ok) {
	u64 dw1, dw2;
	dw1 = readDWordThr(brs, in, ok);
	dw2 = readDWordThr(brs, in, ok);
	return dw1 | (dw2 << 32);
}

