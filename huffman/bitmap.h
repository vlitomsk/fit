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

// Вывести карту в файл (выводятся НЕ символы '1'/'0')
extern void printBitmap(Bitmap *b, FILE *out);
extern void sprintBitmap(Bitmap *b); // для отладки

/* Для чтения внутри битовой карты есть отдельный указатель. */
// Прочитать бит/байт c места последнего чтения, 
// если error равен 1, то произошла ошибка чтения (дошли до конца карты, ...)
extern u8 readBit(Bitmap *b, int *error); 
extern u8 readByte(Bitmap *b, int *error);
extern void resetReads(Bitmap *b); // сбросить указатели чтения 

extern inline u8 setBit(u8 a, u8 bit_no, u8 val);
extern inline u8 getBit(u8 a, u8 bit_no);

#endif
