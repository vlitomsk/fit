#ifndef BITMAP_H
#define BITMAP_H

#include "types.h"
#include <stdio.h>

#define BUFFER_SZ_BYTES (1024)
// в битах
#define BUFFER_SIZE (BUFFER_SZ_BYTES << 3)

typedef struct Bitmap_s {
	u8 buf[BUFFER_SIZE];
	u64 bits_used;
	u64 rd_pos; // reader position in bits
	struct Bitmap_s *next;
} Bitmap;

/* Использует дин.память, не забывать про freeBitmap */
extern Bitmap* newBitmap(void); 
extern void freeBitmap(Bitmap *b);

extern u64 bitmapSize(Bitmap *b); // Размер битовой карты в битах

// Дописать в конец карты бит/байт
extern Bitmap* writeBit(u8 bit, Bitmap *b);
extern Bitmap* writeByte(u8 byte, Bitmap *b);
extern Bitmap* writeDWord(u32 dword, Bitmap *b);

// Вывести карту в файл (выводятся НЕ символы '1'/'0')
extern void printBitmap(Bitmap *b, FILE *out);
extern void sprintBitmap(Bitmap *b); // для отладки

/* Для чтения внутри битовой карты есть отдельный указатель. */
// Прочитать бит/байт c места последнего чтения, 
// если error равен 1, то произошла ошибка чтения (дошли до конца карты, ...)
extern u8 readBit(Bitmap *b, int *error); 
extern u8 readByte(Bitmap *b, int *error);
extern u32 readDWord(Bitmap *b, int *error);
extern void resetReads(Bitmap *b); // сбросить указатели чтения 

#define setBit(a, bit_no, val) ((a) = ((val) ? (a) | (1 << (bit_no)) : (a) & ~(1 << (bit_no))))
#define getBit(a, bit_no) (((a) >> (bit_no)) & 1)

typedef struct {
	u8 len; // used bits. first, LSB is used, ..., MSB is last used bit
	u8 x;
} BRWState; // binary read/write state
/* bws -- bit write state. tut mozhet hranitsa nezapisanniy ostatok */
/* pri pervom vizove funkcii bws = 0 */
extern BRWState* initBRWS(void);
extern void writeBitThr(u8 bit, BRWState *bws, FILE *out);
extern void writeByteThr(u8 byte, BRWState *bws, FILE *out);
extern void writeDWordThr(u32 dword, BRWState *bws, FILE *out);
extern void writeDDWordThr(u64 ddword, BRWState *bws, FILE *out);
extern void flushBitWriter(BRWState *bws, FILE *out);

extern u8 readBitThr(BRWState *brs, FILE *in, int *ok);
extern u8 readByteThr(BRWState *brs, FILE *in, int *ok);
extern u32 readDWordThr(BRWState *brs, FILE *in, int *ok);
extern u64 readDDWordThr(BRWState *brs, FILE *in, int *ok);

#endif
