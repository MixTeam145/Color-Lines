#include <iostream>
#include <conio.h>
#include "Game.h"

int main() {
	Game g;
	g.start_game();
	while (!g.game_over()) {
		g.handle_turn();
	}
	return 0;
}