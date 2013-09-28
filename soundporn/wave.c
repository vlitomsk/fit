#include <stdio.h>
#include <stdlib.h>
#include "wave.h"

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
	// тут должны быть 4 байта под размер chunk (little-endian)
	fputs("WAVE", fp); // Big endian
	fputs("fmt ", fp); // 4 subchunk1 id // Big endian
	fputs("\x10\x00\x00\x00", fp); // 4 subchunk1 size
	fputs("\x01\x00", fp); // 2 audio format (PCM)
	fputs("\x01\x00", fp); // 2 num channels
	fputs("\x40\x1f\x00\x00", fp); // 4 sample rate (8kHz)
	fputs("\x40\x1f\x00\x00", fp); // 4 byte rate (for 8-bit PCM equals sample rate)
	fputs("\x01\x00", fp); // 2 block align
	fputs("\x08\x00", fp); // 2 bits per sample
	fputs("data", fp); // 4 subchunk2 id
	// here 4 bytes = size of data in bytes
	// then data, which is filled using push_wave_unit function

	return ret;
}

WaveFile push_wave_unit(int u, WaveFile f) {
	f.size++;
	fputc(u & 0xff, f.fp);

	return f;
}

void put_little_int(int i, FILE *fp) {
	fputc(i >> 24, fp);
	fputc((i >> 16) & 0xff, fp);
	fputc((i >> 8) & 0xff, fp);
	fputc(i & 0xff, fp);
}

void close_wave_file(WaveFile f) {
	fseek(f.fp, 4, SEEK_SET);
	put_little_int(f.size + 24, f.fp); // chunk size
	fseek(f.fp, 0x28, SEEK_SET);
	put_little_int(f.size, f.fp); // subchunk2 size
	fclose(f.fp);
}
