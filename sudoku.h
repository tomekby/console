#pragma once

#include "console.h"
#include <cassert>
#include <cstdint>

class list;

namespace sudoku {

	// Plansza do gry
	const console::string empty_board[19] = {
		_("╔════╤════╤════╦════╤════╤════╦════╤════╤════╗"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╟────┼────┼────╫────┼────┼────╫────┼────┼────╢"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╟────┼────┼────╫────┼────┼────╫────┼────┼────╢"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╠════╪════╪════╬════╪════╪════╬════╪════╪════╣"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╟────┼────┼────╫────┼────┼────╫────┼────┼────╢"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╟────┼────┼────╫────┼────┼────╫────┼────┼────╢"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╠════╪════╪════╬════╪════╪════╬════╪════╪════╣"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╟────┼────┼────╫────┼────┼────╫────┼────┼────╢"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╟────┼────┼────╫────┼────┼────╫────┼────┼────╢"),
		_("║    │    │    ║    │    │    ║    │    │    ║"),
		_("╚════╧════╧════╩════╧════╧════╩════╧════╧════╝")
	};

	// @todo enkapsulacja...
	class cell {
	public:
		cell(const unsigned x, const unsigned y, const unsigned val) : x(x), y(y), val(val != 0 ? std::to_wstring(val) : _("")) {
			for (unsigned i = 0; i < 9; ++i) comments[i] = -1;
		}
		cell() : cell(0, 0, 0) {};
		inline cell operator=(const console::string &value);
		cell operator=(const cell old);
		cell operator=(const cell *old);
		// Koordynaty komórki
		unsigned x, y;
		short comments[9];
		// Wartość komórki (zmieniana przez operator=)
		console::string val;
	};

	/**
	 * Klasa obsługująca planszę sudoku
	 */
	class board {
		friend class list;
	public:
		enum class direction { LEFT, RIGHT, TOP, BOTTOM, NONE };
		board(const unsigned short start_x, const unsigned short start_y, const bool draw_at_startup = true)
			: START_X(start_x), START_Y(start_y), cur_pos(0, 0), draw_at_startup(draw_at_startup) {

			if (draw_at_startup) {
				console::cls();

				// Wypisanie planszy
				draw_board();
			}

			// Zerowanie zawartości sudoku
			for (unsigned i = 0; i < 9; ++i) {
				for (unsigned j = 0; j < 9; ++j) {
					this->values[i][j] = cell(i, j, 0);
				}
			}
		}
		board() : board(0, 0) {}
		void init_vals(const cell *cells, const unsigned short size);
		void print(const console::string &val);
		void print(const unsigned short x, const unsigned short y, const console::string &val);
		const cell get_cell(const unsigned short x, const unsigned short y);
		const console::string get_value(const unsigned short x, const unsigned short y);
		const short get_num_value(const unsigned short x, const unsigned short y);
		void goto_cell(const sudoku::board::direction d);
		const bool conflicts(const unsigned short v);
		board& redraw_all(const bool clear_screen = true);
		board& operator=(const board &old);
		const bool* curr_prompt();
		const pair<console::cursor, unsigned short> explicit_prompt();
		// Start tekstu za planszą
		inline const unsigned additionals_start() {
			return 19 + START_Y;
		}
		// Włączenie kolorowania określonego znaku
		inline void toggle_alternative(const console::string &number) {
			_use_alternative = number;
		}
		const pair<unsigned short*, unsigned short> get_comments(const unsigned short x, const unsigned short y);
		const pair<unsigned short*, unsigned short> get_comments();
		void add_comment(const short v);
		void remove_comment(const short v);
		// Sprawdzanie czy to nie jest już koniec gry
		inline const bool is_endgame() {
			for (size_t i = 0; i < 9; ++i) {
				for (size_t j = 0; j < 9; ++j) {
					if (get_num_value(i, j) == -1)
						return false;
				}
			}
			return true;
		}
	private:
		void draw_board();
		void print(const unsigned short x, const unsigned short y, const console::string &val, const bool init);
		const bool conflicts(const unsigned short x, const unsigned short y, const unsigned short v);
		const bool conflicts_row(const unsigned short x, const unsigned short y, const short val);
		const bool conflicts_col(const unsigned short x, const unsigned short y, const short val);
		const bool conflicts_square(const unsigned short x, const unsigned short y, const short val);
		const unsigned short get_unconflicting(const unsigned short x, const unsigned short y);
		const bool check_if_conflicting(const unsigned short x, const unsigned short y, const unsigned short k, unsigned short &non_conflicting, unsigned short &c_count);
		const unsigned short check_unconflicting_for_empty(const unsigned short x, const unsigned short y);
		inline const bool use_alternative(const console::string &val) {
			return (val == _use_alternative || val == _use_alternative + _("_")) && val != console::string();
		}
		void add_comment(const unsigned short x, const unsigned short y, const short v);
		void remove_comment(const unsigned short x, const unsigned short y, const short v);

		// Używanie alternatywnego stylu (np. do wyróżnienia)
		console::string _use_alternative = console::string();
		// Koordynaty początkowe planszy
		unsigned short START_X, START_Y;
		// Wartości w poszczególnych komórkach
		sudoku::cell values[9][9];
		// Pozycja kursora na planszy
		console::cursor cur_pos;
		bool draw_at_startup;
	};
} // End namespace sudoku