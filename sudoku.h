#pragma once

#include "console.h"
#include <cassert>
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

	class cell {
	public:
		cell(const unsigned x, const unsigned y, const unsigned val) : x(x), y(y), val(std::to_wstring(val)) {}
		// Koordynaty komórki
		const unsigned x, y;
		// Wartość komórki
		const console::string val;
		// Komentarz do komórki
		const console::string comment;
	};

	/**
	 * Klasa obsługująca planszę sudoku
	 */
	class board {
	public:
		board(const unsigned short start_x, const unsigned short start_y, const short counter_x = -1, const unsigned short counter_y = 0)
			: START_X(start_x), START_Y(start_y), COUNTER_X(counter_x), COUNTER_Y(counter_y), move_counter(-1) {

			// Inicjalizacja licznika
			++(*this);

			// Wypisywanie planszy
			for (unsigned i = 0; i < 19; ++i) {
				// Przesunięcie kursora na odpowiednie miejsce poniżej
				console::gotoxy(start_x, start_y + i);
				console::print(empty_board[i]);
			}
			// Zerowanie zawartości sudoku
			for (unsigned i = 0; i < 9; ++i) {
				for (unsigned j = 0; j < 9; ++j) {
					this->values[i][j] = _("  ");
				}
			}
		}
		board() : board(0, 0) {}
		void init_vals(const cell *cells, const unsigned short size);
		void print(const int x, const int y, const console::string &val);
		void change(const int x, const int y, const console::string &val);
		void clear(const int x, const int y);
		board& operator++();
	private:
		void print(const int x, const int y, const console::string &val, const bool init);

		// Koordynaty początkowe planszy
		const unsigned short START_X, START_Y;
		// Koordynaty licznika ruchów
		const short COUNTER_X;
		const unsigned short COUNTER_Y;
		// Tekst wyświetlany w miejscu licznika ruchów
		const console::string COUNTER_TEXT = _("Liczba ruchów: ");
		// Ilość wykonanych ruchów w grze
		short move_counter;
		// Wartości w poszczególnych komórkach
		console::string values[9][9];
	};
} // End namespace sudoku