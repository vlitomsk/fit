#include <stdio.h>
#include <stdlib.h>
#include "player.h"
#include "wave.h"

main() {
	init_player();
	//play_note(G, 3, 100.0);
	// int i;

	// for (i = 1; i <= 8; ++i) {
	// 	play_note(E, i, 1);
	// 	play_note(G, i, 1);
	// 	play_note(A, i, 1);
	// 	play_note(sil, 0, 0.5);
	// 	play_note(E, i, 1);
	// 	play_note(G, i, 1);
	// 	play_note(Hb, i, 0.5);
	// 	play_note(A, i, 1);
	// 	play_note(sil, 0, 1);
	// 	play_note(E, i, 1);
	// 	play_note(G, i, 1);
	// 	play_note(A, i, 1);
	// 	play_note(sil, 0, 0.5);
	// 	play_note(G, i, 1);
	// 	play_note(E, i, 1);
	// 	play_note(sil, 0, 1);	
	// }
	
	// set_tempo(100);
	// play_sequence("E41 G41 A41 002 E41 G41 Hb42 A41 001 E41 G41 A41 002 G41 E41 001"); // smoke on the water
	// play_sequence("A24 C34 D34 Eb34 E34 G34 A34 C44 D44 Eb44 E44 G44 A44 C54"); // A blues scale

	WaveFile wav = open_wave_file("./shiroka.wav");
	set_tempo(35);
	play_sequence("A32 034 D42 004 Db44 H34 Db44 D44 E44 D42 A31 001", &wav); // широка страна моя родная
	play_sequence("A32 034 D42 004 Db44 H34 Db44 D44 E44 D42 A31 001", &wav); // широка страна моя родная
	play_sequence("A32 034 D42 004 Db44 H34 Db44 D44 E44 D42 A31 001", &wav); // широка страна моя родная
	close_wave_file(wav);

	// WaveFile wav = open_wave_file("./test.wav");
	// wav = push_wave_unit(0xff, wav);
	// wav = push_wave_unit(0, wav);
	// wav = push_wave_unit(0x11, wav);
	// close_wave_file(wav);

	return 0;
}
