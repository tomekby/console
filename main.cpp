#include "console.h"
#include "sudoku.h"

/**
 * @TODO:
 *      - możliwość ustawiania wielkośći konsoli
 *      - naprawienie kolorowania i polskich znaków na linuksie (!)
 *		- scroll
 *		- pozycja kursora na Linuksie
 *		- input
 */
int main()
{
	console::InitConio2();
	const console::string TITLE = _("Gra sudoku");
	console::title(TITLE);
	console::cls(); // Czyszczenie ekranu jakby coś było wcześniej uruchamiane oraz dla zmiany ew. koloru tła
	sudoku::board sudoku(5, 1);
	// Drukowanie planszy do gry
	const sudoku::cell initial[] = {
		sudoku::cell(0, 0, 2), sudoku::cell(0, 2, 6), sudoku::cell(2, 2, 7),
		sudoku::cell(3, 0, 6), sudoku::cell(5, 0, 7), sudoku::cell(5, 2, 1),
		sudoku::cell(6, 0, 5), sudoku::cell(7, 1, 9), sudoku::cell(8, 1, 6), sudoku::cell(6, 2, 3),
		sudoku::cell(1, 3, 5), sudoku::cell(1, 4, 7),
		sudoku::cell(3, 3, 7), sudoku::cell(4, 3, 3), sudoku::cell(5, 3, 2),
		sudoku::cell(3, 5, 1), sudoku::cell(4, 5, 8), sudoku::cell(5, 5, 9),
		sudoku::cell(7, 4, 2), sudoku::cell(7, 5, 7),
		sudoku::cell(2, 6, 3), sudoku::cell(0, 7, 8), sudoku::cell(1, 7, 4), sudoku::cell(2, 8, 5),
		sudoku::cell(3, 6, 5), sudoku::cell(3, 7, 2), sudoku::cell(5, 7, 6),
		sudoku::cell(6, 6, 6), sudoku::cell(8, 6, 4), sudoku::cell(8, 8, 8),
	};
	sudoku.init_vals(initial, 30);

	console::print(_("Przykładowy tekst wydrukowany w konsoli i pokolorowany"), _("Przykładowy"));
	int ch;
	do {
		ch = console::getch();
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
