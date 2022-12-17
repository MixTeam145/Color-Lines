#pragma once
#include "Console.h"

const int startX = 18;
const int startY = 3;

const int OCCUPIED = -1;
const int BLANK = -2;

const COLORS FIELD_COLOR = B_D_WHITE;

struct Cell {
	int x, y;
	COLORS color;
	void init(int t_x, int t_y, COLORS t_color) {
		x = t_x;
		y = t_y;
		color = t_color;
	}
};

class Game {
public:
	Game();
	void handle_turn();
	void start_game();
	void print_cell(const Cell* cell) const;
	void update_info() const;
	void generate(int n);
	bool check_bounds(const int x, const int y);
	bool make_path(const int x, const int y);
	bool move_to(const int x, const int y);
	int check_line(const int x, const int y, const int i, const int j);
	void destroy_balls(const int x, const int y, const int k, const int i, const int j);
	void destroy_lines();
	bool game_over();
private:
	int m_x = startX, m_y = startY;
	Cell map[9][9];
	Cell* selected = nullptr;
	size_t stime;
	int score, empty_count;
	bool destroy_flag = false, moved = false;
	int px[81]{}, py[81]{}, len;
};
