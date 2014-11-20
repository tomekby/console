#include "sudoku.h"

namespace sudoku {
	board& board::operator++() {
		const console::cursor TMP = console::get_cursor_pos();
		const console::string PRINTED = COUNTER_TEXT + std::to_wstring(++move_counter);
		// Miejsce licznika
		const unsigned POS_X = COUNTER_X < 0 ? 80 - PRINTED.length() - 1 : COUNTER_X;
		console::gotoxy(POS_X, COUNTER_Y);
		// Wypisanie wartoœci licznika
		console::print(PRINTED, console::ALL);

		// Powrót do poprzedniej pozycji kursora
		console::gotoxy(TMP);
		return *this;
	}

	void board::init_vals(const cell *cells, const unsigned short size) {
		for (unsigned short i = 0; i < size; ++i) {
			print(cells[i].x, cells[i].y, cells[i].val, true);
		}
	}

	void board::print(const int x, const int y, const console::string &val, const bool init) {
		assert(x < 9);
		assert(y < 9);
		assert(val.length() <= 2);

		// Pozycja kursora przed wpisaniem wartoœci do komórki
		const console::cursor TMP = console::get_cursor_pos();
		// Wpisanie wartoœci do komórki
		console::gotoxy(START_X + 2 + 5 * x, START_Y + 1 + 2 * y);
		console::print(val, console::ALL);
		values[x][y] = val;
		if (!init) // Jeœli to nie jest inicjalizacja, dodanie ruchu do licznika
			++(*this);

		// Powrót do poprzedniej pozycji
		console::gotoxy(TMP);
	}

	void board::print(const int x, const int y, const console::string &val) {
		print(x, y, val, false);
	}

	void board::clear(const int x, const int y) {
		print(x, y, _("  "));
	}

	void board::change(const int x, const int y, const console::string &val) {
		clear(x, y);
		print(x, y, val);
	}
} // End namespace sudoku