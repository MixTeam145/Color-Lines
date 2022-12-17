#include "Game.h"
#include <random>
#include <time.h>
#include <iostream>
#include <conio.h>

Game::Game() {
	srand(time(0));
	InitConsole("Game", 50, 50);
	VisibleCursor(false);
	MoveCursor(18, 2);
	ColorPrint(F_L_WHITE, "Color Lines");
	MoveCursor(15, 5);
	ColorPrint(F_L_WHITE, "Press Enter to start");
	std::cin.get();
}

void Game::start_game() {
	ClearConsole();
	score = 0;
	empty_count = 81;
	VisibleCursor(false);
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			map[i][j].init(j, i, FIELD_COLOR);
		}
	}
	MoveCursor(3, startY - 2);
	ColorPrint(F_L_WHITE, "Selected: ");
	MoveCursor(startX + 5, startY - 2);
	ColorPrint(F_L_WHITE, "Score: %d", score);
	for (int i = 0; i < 9; ++i) {
		int x{ startX }, y{ startY + i };
		MoveCursor(x, y);
		for (int j = 0; j < 8; ++j) {
			ColorPrint(FIELD_COLOR, "  ");
		}
		ColorPrint(FIELD_COLOR, " ");
	}
	MoveCursor(m_x, m_y);
	VisibleCursor(true);
	generate(5);
}

void Game::print_cell(const Cell* cp) const {
	VisibleCursor(false);
	MoveCursor(startX + cp->x * 2, startY + cp->y);
	if (cp->color != FIELD_COLOR) {
		ColorPrint(cp->color | FIELD_COLOR, "*");
	}
	else {
		ColorPrint(FIELD_COLOR, " ");
	}
	MoveCursor(m_x, m_y);
	VisibleCursor(true);
}

void Game::update_info() const {
	VisibleCursor(false);
	MoveCursor(13, startY - 2);
	ColorPrint(selected ? static_cast<COLORS>(selected->color % 8) : BLACK, "*");
	MoveCursor(startX + 12, startY - 2);
	ColorPrint(F_L_WHITE, "%d", score);
	MoveCursor(m_x, m_y);
	VisibleCursor(true);
}

void Game::handle_turn() {
	if (selected && clock() - stime >= 500) {
		selected->color = static_cast<COLORS>((selected->color + 8) % 16);
		stime = clock();
		print_cell(selected);
	}
	if (_kbhit()) {
		char c = _getch();
		switch (c) {
		case -32:
			switch (_getch()) {
			case 75:
				if (m_x > startX) MoveCursor(m_x -= 2, m_y);
				break;
			case 77:
				if (m_x < startX + 16) MoveCursor(m_x += 2, m_y);
				break;
			case 80:
				if (m_y < startY + 8) MoveCursor(m_x, m_y += 1);
				break;
			case 72:
				if (m_y > startY) MoveCursor(m_x, m_y -= 1);
				break;
			}
			break;
		case '\r':
			if (!selected) {
				if (map[m_y - startY][(m_x - startX) / 2].color != FIELD_COLOR) {
					selected = &map[m_y - startY][(m_x - startX) / 2];
					stime = clock();
					print_cell(selected);
					update_info();
				}
			}
			else {
				if (map[m_y - startY][(m_x - startX) / 2].color == FIELD_COLOR && move_to((m_x - startX) / 2, m_y - startY)) {
					moved = true;
					selected = nullptr;
					update_info();
				}
			}
			break;
		case 27:
			if (selected) {
				selected->color = static_cast<COLORS>(selected->color % 8);
				print_cell(selected);
				selected = nullptr;
				update_info();

			}
			break;
		}
		if (moved) {
			moved = false;
			Sleep(400);
			destroy_lines();
			update_info();
			if (!destroy_flag) {
				generate(3);
				Sleep(400);
				destroy_lines();
				update_info();
			}
		}

	}
}

void Game::generate(int n) {
	n = min(n, empty_count);
	for (int i = 0; i < n; ++i) {
		int r;
		do {
			r = rand() % 81;
		} while (map[r / 9][r % 9].color != FIELD_COLOR);
		map[r / 9][r % 9].color = static_cast<COLORS>(rand() % 6 + 1);
		print_cell(&map[r / 9][r % 9]);
		--empty_count;
	}
}

bool Game::check_bounds(const int x, const int y) {
	return x >= 0 && x <= 8 && y >= 0 && y <= 8;
}

bool Game::make_path(const int bx, const int by) {
	int dx[4]{ 1, 0, -1, 0 };
	int dy[4]{ 0, 1, 0, -1 };
	int grid[9][9];
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			if (map[i][j].color == FIELD_COLOR) grid[i][j] = BLANK;
			else grid[i][j] = OCCUPIED;
		}
	}
	grid[selected->y][selected->x] = 0;
	int d = 0;
	bool stop;
	do {
		stop = true;
		for (int i = 0; i < 9; ++i) {
			for (int j = 0; j < 9; ++j) {
				if (grid[i][j] == d) {
					for (int k = 0; k < 4; ++k) {
						int iy{ i + dy[k] }, ix{ j + dx[k] };
						if (check_bounds(ix, iy) && grid[iy][ix] == BLANK) {
							stop = false;
							grid[iy][ix] = d + 1;
						}
					}
				}
			}
		}
		++d;
	} while (!stop && grid[by][bx] == BLANK);

	if (grid[by][bx] == BLANK) return false;

	len = grid[by][bx];
	d = len;
	int x{ bx }, y{ by };
	while (d > 0) {
		px[d] = x;
		py[d] = y;
		--d;
		for (int k = 0; k < 4; ++k) {
			int iy{ y + dy[k] }, ix{ x + dx[k] };
			if (check_bounds(ix, iy) && grid[iy][ix] == d) {
				x += dx[k];
				y += dy[k];
				break;
			}
		}
	}
	px[0] = selected->x;
	py[0] = selected->y;
	return true;
}

bool Game::move_to(const int x, const int y) {
	bool res = make_path(x, y);
	if (res) {
		map[py[0]][px[0]].color = static_cast<COLORS>(selected->color % 8);
		for (int i = 1; i <= len; ++i) {
			map[py[i]][px[i]].color = map[py[i - 1]][px[i - 1]].color;
			map[py[i - 1]][px[i - 1]].color = FIELD_COLOR;
			print_cell(&map[py[i]][px[i]]);
			print_cell(&map[py[i - 1]][px[i - 1]]);
			Sleep(400);
		}
	}
	return res;
}

int Game::check_line(const int x, const int y, const int i, const int j) {
	int dx{ x }, dy{ y }, k{};
	while (map[x][y].color == map[dx][dy].color) {
		if (check_bounds(dx, dy)) {
			dx += i;
			dy += j;
			++k;
		}
		else break;
	}
	return k;
}

void Game::destroy_balls(const int x, const int y, const int k, const int i, const int j) {
	int dx{ x }, dy{ y };
	for (int p = 0; p < k; ++p) {
		map[dx][dy].color = FIELD_COLOR;
		print_cell(&map[dx][dy]);
		dx += i;
		dy += j;
	}
	score += k;
	empty_count += k;
	destroy_flag = true;
}

void Game::destroy_lines() {
	destroy_flag = false;
	int di[4]{ 1, 0, 1, -1 };
	int dj[4]{ 0, 1, 1, 1 };
	for (int i = 0; i < 9; ++i) {
		for (int j = 0; j < 9; ++j) {
			if (map[i][j].color != FIELD_COLOR) {
				for (int k = 0; k < 4; ++k) {
					int count{ check_line(i, j, di[k], dj[k]) };
					if (count > 4) {
						destroy_balls(i, j, count, di[k], dj[k]);
						break;
					}
				}
			}
		}
	}
}

bool Game::game_over() {
	if (!empty_count) {
		VisibleCursor(false);
		ClearConsole();
		MoveCursor(20, 3);
		ColorPrint(F_L_WHITE, "GAME OVER!");
		MoveCursor(19, 4);
		ColorPrint(F_L_WHITE, "Your score: ");
		ColorPrint(F_D_BLUE, "%d", score);
		MoveCursor(20, 6);
		ColorPrint(F_L_WHITE, "Play again?");
		MoveCursor(15, 8);
		ColorPrint(F_L_WHITE, "Yes(Enter)    No(Esc)");
		char c;
		do {
			c = _getch();
		} while (c != '\r' && c != 27);
		if (c == '\r') start_game();
	}
	return !empty_count;
}
