#include "menu.h"

/**
 * @TODO:
 *      - możliwość ustawiania tytułu konsoli
 *      - możliwość ustawiania wielkośći konsoli
 *      - naprawienie kolorowania i polskich znaków na linuksie (!)
 */

int main()
{
	menu::cls(); // Czyszczenie ekranu jakby coś było wcześniej uruchamiane oraz dla zmiany ew. koloru tła
	menu::print(L"Przykładowy tekst wydrukowany w konsoli i pokolorowany :)", L"Przykładowy");
	menu::print(L"ĘęÓóĄąŚśŁłŻżŹźĆćŃń", menu::ALL);
	menu::print(L"ĘęÓóĄąŚśŁłŻżŹźĆćŃń", 0);
	menu::pause();
	menu::cls();
	menu::print(L"Inne zdanie wydrukowane w konsoli i pokolorowane :)", L"zdanie");
	menu::pause();

	return 0;
}
