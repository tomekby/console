#include "console.h"

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
	console::title(_("Klasa wspomagająca konsolę"));
	console::cls(); // Czyszczenie ekranu jakby coś było wcześniej uruchamiane oraz dla zmiany ew. koloru tła
	console::print(_("Przykładowy tekst wydrukowany w konsoli i pokolorowany :)"), _("Przykładowy"));
	console::print(_("ĘęÓóĄąŚśŁłŻżŹźĆćŃń"), console::ALL);
	console::print(_("ĘęÓóĄąŚśŁłŻżŹźĆćŃń"), 0);
	console::pause();
	console::cls();
	console::print(_("Inne zdanie wydrukowane w konsoli i pokolorowane :)"), _("zdanie"));
	console::print(_("jakiś tekst \u2550 \u2551 \u2552 \u2553 \u2554 \u2555 \u2556 \u2557 \u2558 \u2559"));
	// Przykład przesunięcia kursora
	console::gotoxy(10, 10);
	auto c = console::get_cursor_pos();
	//console::print(_("(") + std::to_wstring(c.x) + _("; ") + std::to_wstring(c.y) + _(")"));
	console::pause();

	return 0;
}
