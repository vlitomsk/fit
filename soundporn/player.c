#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "player.h"

static volatile int tempo;
int ticks_per_semibreve; // тиков с частотой дискретизации за целую ноту

int note_freqs[OCTAVES][NOTES];

void fill_freqs(void) {
	// for Fifth octave (C8, C#8, ...)
	int l = OCTAVES - 1;
	note_freqs[l][C]  = 8372;
	note_freqs[l][Db] = 8870;
	note_freqs[l][D]  = 9397;
	note_freqs[l][Eb] = 9956;
	note_freqs[l][E]  = 10548;
	note_freqs[l][F]  = 11175;
	note_freqs[l][Gb] = 11840;
	note_freqs[l][G]  = 12544;
	note_freqs[l][Ab] = 13290;
	note_freqs[l][A]  = 14080;
	note_freqs[l][Hb] = 14917;
	note_freqs[l][H]  = 15804;

	int i, j;
	for (i = l - 1; i >= 0; --i) {
		for (j = 0; j < NOTES; ++j) 
			note_freqs[i][j] = note_freqs[i + 1][j] >> 1;
	}
}

void set_tempo(int t) {
	tempo = t;
	ticks_per_semibreve = PCM_FREQ * 60 / tempo;
}

void init_player() {
	fill_freqs();
	set_tempo(200);
}

bool fade_on = true, vibrato_on = true;
void set_fade(bool enable) {
	fade_on = enable;
}

void set_vibrato(bool enable) {
	vibrato_on = enable;
}

void setup_vibrato(double k) {
	
}

void setup_fade(double percent_to) {

}

char calc_char(int i, int ticks) {
	double tmp = 255.0;
	if (vibrato_on)
		tmp *= fabs(sin((double)i / 300));
	if (fade_on)
		tmp *= (double)(ticks - i) / (double)ticks;

	return (char)floor(tmp);
}

/* 
  octave is from interval 1..8, 
  duration is relative duration of note (1.0 is semibreve)
*/
void play_note(tnote note, int octave, float duration, WaveFile *wav) {
	int i,
		ticks = floor(duration * ticks_per_semibreve);
	int modulo = PCM_FREQ / note_freqs[octave - 1][note];

	for (i = 0; i < ticks; ++i) {
		char char_val = 0;
		if (i % modulo == 0 && note != sil) 
			char_val = calc_char(i, ticks);
		
		if (wav)
			*wav = push_wave_unit(char_val, *wav);
		else
			putchar(char_val);
	}
}

tnote parse_note(char char_val) {
	tnote note = sil;
	switch (char_val) {
		case 'C': note = C;
			break;
		case 'D': note = D;
			break;
		case 'E': note = E;
			break;
		case 'F': note = F;
			break;
		case 'G': note = G;
			break;
		case 'A': note = A;
			break;
		case 'H': note = H;
			break;
		case '0': note = sil;
			break;
		default:
			fprintf(stderr, "Error parsing note %c\n", char_val);
			break;
	}

	return note;
}

/* 
 Format:
   <Note><Octave><Duration>
   Note: C, Db, D, ...
   Octave: [1..8]
   Duration: 1, 2, 4, ... - semibreve, half, quarter, ...
*/
void play_token(const char *tok, int len, WaveFile *wav) {
	tnote note;
	int octave;
	float duration;
	note = parse_note(tok[0]);
	octave = tok[1] - '0';
	if (tok[1] == 'b') {
		--note;
		octave = tok[2] - '0';
		duration = (float)(tok[3] - '0');
	} else if (tok[1] == '#') {
	  ++note;
	  octave = tok[2] - '0';
	  duration = (float)(tok[3] - '0');
  } else
		duration = (float)(tok[2] - '0');
	duration = 1.0 / duration;

	// fprintf(stderr, "Note: %d; Octave: %d; Duration: %f\n", note, octave, duration);

	play_note(note, octave, duration, wav);
}

// splits seq by spaces and applies play_token to each token (see fmt there)
void play_sequence(const char *seq, WaveFile *wav) {
	if (strlen(seq) < 3) 
		return;
	int last_space = 0,
		i;
	for (i = 0; i < strlen(seq); ++i) {
		if (seq[i] == ' ') {
			play_token(seq + last_space, i - last_space, wav);
			last_space = i + 1;
		}
	}
	play_token(seq + last_space, i - last_space, wav);
}
