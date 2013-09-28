#ifndef WAVE_H_INCLUDED
#define WAVE_H_INCLUDED

#include <stdio.h>

typedef struct WaveFile_ {
	FILE *fp;
	int size;
} WaveFile;

// only 8bit 8kHz PCM support!
extern WaveFile open_wave_file(const char* path);
extern WaveFile push_wave_unit(int u, WaveFile f);
extern void close_wave_file(WaveFile f);

#endif