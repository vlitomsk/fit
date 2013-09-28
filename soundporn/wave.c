#include <stdio.h>
#include <stdlib.h>
#include "wave.h"

void fput2c(char c1, char c2, FILE *fp) {
	fputc(c1, fp);
	fputc(c2, fp);
}

void fput4c(char c1, char c2, char c3, char c4, FILE *fp) {
	fput2c(c1, c2, fp);
	fput2c(c3, c4, fp);
}

WaveFile open_wave_file(const char* path) {
	FILE *fp = fopen(path, "wb+");
	WaveFile ret;
	ret.fp = fp;
	ret.size = 0;
	if (!fp) {
		fprintf(stderr, "Can't open file '%s' for writing\n", path);
		return ret;
	}

	fputs("RIFF", fp); // Big endian
	fputs("xxxx", fp);// тут должны быть 4 байта под размер chunk (little-endian)
	fputs("WAVE", fp); // Big endian
	fputs("fmt ", fp); // 4 subchunk1 id // Big endian

	fput4c(0x10, 0x00, 0x00, 0x00, fp); // 4 subchunk1 size
	fput2c(0x01, 0x00, fp); // 2 audio format (PCM)
	fput2c(0x01, 0x00, fp); // 2 num channels
	fput4c(0x40, 0x1f, 0x00, 0x00, fp); // 4 sample rate (8kHz)
	fput4c(0x40, 0x1f, 0x00, 0x00, fp); // 4 byte rate (for 8-bit PCM equals sample rate)
	fput2c(0x01, 0x00, fp); // 2 block align
	fput2c(0x08, 0x00, fp); // 2 bits per sample
	fputs("data", fp); // 4 subchunk2 id
	fputs("xxxx", fp); // here 4 	bytes = size of data in bytes
	// then data, which is filled using push_wave_unit function

	return ret;
}

WaveFile push_wave_unit(int u, WaveFile f) {
	f.size++;
	fputc(u & 0xff, f.fp);

	return f;
}

void put_little_int(int i, FILE *fp) {
	fputc(i & 0xff, fp);
	fputc((i >> 8) & 0xff, fp);
	fputc((i >> 16) & 0xff, fp);
	fputc(i >> 24, fp);
}

void close_wave_file(WaveFile f) {
	fseek(f.fp, 4, SEEK_SET);
	put_little_int(f.size + 36, f.fp); // chunk size
	fseek(f.fp, 0x28, SEEK_SET);
	put_little_int(f.size, f.fp); // subchunk2 size
	fclose(f.fp);
}
