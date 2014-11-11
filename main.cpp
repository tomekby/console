#include "console.h"
#include <cassert>

/**
 * @TODO:
 *      - możliwość ustawiania wielkośći konsoli
 *      - naprawienie kolorowania i polskich znaków na linuksie (!)
 *		- scroll
 *		- pozycja kursora na Linuksie
 *		- input
 */
// Plansza do gry
const string board[19] = {
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

class sudoku_cell {
public:
	sudoku_cell(const unsigned x, const unsigned y, const unsigned val) : x(x), y(y), val(std::to_wstring(val)) {}
	// Koordynaty komórki
	const unsigned x, y;
	// Wartość komórki
	const string val;
};

class sudoku {
public:
	sudoku(const unsigned short start_x, const unsigned short start_y, const short counter_x = -1, const unsigned short counter_y = 0)
		: START_X(start_x), START_Y(start_y), COUNTER_X(counter_x), COUNTER_Y(counter_y), move_counter(-1) {

		// Inicjalizacja licznika
		++(*this);

		// Wypisywanie planszy
		for (unsigned i = 0; i < 19; ++i) {
			// Przesunięcie kursora na odpowiednie miejsce poniżej
			console::gotoxy(start_x, start_y + i);
			console::print(board[i]);
		}
		// Zerowanie zawartości sudoku
		for (unsigned i = 0; i < 9; ++i) {
			for (unsigned j = 0; j < 9; ++j) {
				this->values[i][j] = _("  ");
			}
		}
	}
	sudoku() : sudoku(0, 0) {}
	void init_vals(const sudoku_cell *cells, const unsigned short size);
	void print(const int x, const int y, const string &val);
	void change(const int x, const int y, const string &val);
	void clear(const int x, const int y);
	sudoku& operator++();
private:
	void print(const int x, const int y, const string &val, const bool init);

	// Koordynaty początkowe planszy
	const unsigned short START_X, START_Y;
	// Koordynaty licznika ruchów
	const short COUNTER_X;
	const unsigned short COUNTER_Y;
	// Tekst wyświetlany w miejscu licznika ruchów
	const string COUNTER_TEXT = _("Liczba ruchów: ");
	// Ilość wykonanych ruchów w grze
	short move_counter;
	// Wartości w poszczególnych komórkach
	string values[9][9];
};

sudoku& sudoku::operator++() {
	const cursor TMP = console::get_cursor_pos();
	const string PRINTED = COUNTER_TEXT + std::to_wstring(++move_counter);
	// Miejsce licznika
	const unsigned POS_X = COUNTER_X < 0 ? 80 - PRINTED.length() - 1 : COUNTER_X;
	console::gotoxy(POS_X, COUNTER_Y);
	// Wypisanie wartości licznika
	console::print(PRINTED, console::ALL);

	// Powrót do poprzedniej pozycji kursora
	console::gotoxy(TMP);
	return *this;
}

void sudoku::init_vals(const sudoku_cell *cells, const unsigned short size) {
	for (unsigned short i = 0; i < size; ++i) {
		print(cells[i].x, cells[i].y, cells[i].val, true);
	}
}

void sudoku::print(const int x, const int y, const string &val, const bool init) {
	assert(x < 9);
	assert(y < 9);
	assert(val.length() <= 2);

	// Pozycja kursora przed wpisaniem wartości do komórki
	const cursor TMP = console::get_cursor_pos();
	// Wpisanie wartości do komórki
	console::gotoxy(START_X + 2 + 5 * x, START_Y + 1 + 2 * y);
	console::print(val, console::ALL);
	values[x][y] = val;
	if ( ! init) // Jeśli to nie jest inicjalizacja, dodanie ruchu do licznika
		++(*this);

	// Powrót do poprzedniej pozycji
	console::gotoxy(TMP);
}

void sudoku::print(const int x, const int y, const string &val) {
	print(x, y, val, false);
}

void sudoku::clear(const int x, const int y) {
	print(x, y, _("  "));
}

void sudoku::change(const int x, const int y, const string &val) {
	clear(x, y);
	print(x, y, val);
}

int main()
{
	console::title(_("Gra sudoku"));
	console::cls(); // Czyszczenie ekranu jakby coś było wcześniej uruchamiane oraz dla zmiany ew. koloru tła
	sudoku board(5, 1);
	// Drukowanie planszy do gry
	const sudoku_cell initial[] = {
		sudoku_cell(0, 0, 2), sudoku_cell(0, 2, 6), sudoku_cell(2, 2, 7),
		sudoku_cell(3, 0, 6), sudoku_cell(5, 0, 7), sudoku_cell(5, 2, 1),
		sudoku_cell(6, 0, 5), sudoku_cell(7, 1, 9), sudoku_cell(8, 1, 6), sudoku_cell(6, 2, 3),
		sudoku_cell(1, 3, 5), sudoku_cell(1, 4, 7),
		sudoku_cell(3, 3, 7), sudoku_cell(4, 3, 3), sudoku_cell(5, 3, 2),
		sudoku_cell(3, 5, 1), sudoku_cell(4, 5, 8), sudoku_cell(5, 5, 9),
		sudoku_cell(7, 4, 2), sudoku_cell(7, 5, 7),
		sudoku_cell(2, 6, 3), sudoku_cell(0, 7, 8), sudoku_cell(1, 7, 4), sudoku_cell(2, 8, 5),
		sudoku_cell(3, 6, 5), sudoku_cell(3, 7, 2), sudoku_cell(5, 7, 6),
		sudoku_cell(6, 6, 6), sudoku_cell(8, 6, 4), sudoku_cell(8, 8, 8),
	};
	board.init_vals(initial, 30);

	console::print(_("Przykładowy tekst wydrukowany w konsoli i pokolorowany :)"), _("Przykładowy"));
	int ch;
	do {
		ch = console::getch();
		console::print(std::to_wstring(ch));
		if (ch == 0) {
			ch = console::getch();
			if (ch == 0x48) console::print(L"up"); // ↑
			else if (ch == 0x50) console::print(L"down"); // ↓
			else if (ch == 0x4b) console::print(L"right"); // →
			else if (ch == 0x4d) console::print(L"left"); // ←
		}
		//else if (ch == ' ') attr = (attr + 1) % 16;
	} while (ch != 'q');

	return 0;
}
