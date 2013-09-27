#include <stdio.h>
#include <stdlib.h>
#include "player.h"

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

	set_tempo(100);
	play_sequence("E41 G41 A41 002 E41 G41 Hb42 A41 001 E41 G41 A41 002 G41 E41 001"); // smoke on the water

	set_tempo(50);
	play_sequence("A34 034 D42 004 Db44 H34 Db44 D44 E44 D42 A32"); // широка страна моя родная

	set_tempo(100);
	play_sequence("A24 C34 D34 Eb34 E34 G34 A34 C44 D44 Eb44 E44 G44 A44 C54"); // A blues scale

	return 0;
}
