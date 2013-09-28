#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#define PCM_FREQ 8000
#define OCTAVES 9
#define NOTES 12

typedef enum {C, Db, D, Eb, E, F, Gb, G, Ab, A, Hb, H, sil} tnote;
typedef enum {false, true} bool;

extern void init_player(void);
extern void play_note(tnote note, int octave, float duration);
extern void play_sequence(const char *seq);
extern void set_tempo(int t);
extern void set_fade(bool enable);
extern void set_vibrato(bool enable);

#endif