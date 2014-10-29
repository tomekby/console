#include "menu.h"

/**
 * @TODO:
 *      - mo¿liwoœæ ustawiania tytu³u konsoli
 *      - mo¿liwoœæ ustawiania wielkoœæi konsoli
 *      - mo¿liwoœæ ustalania kodowania (Windows)
 *      - naprawienie kolorowania i polskich znaków na linuksie (!)
 */

/**
 * Funkcja zamieniaj¹ca literê ma³¹ na wielk¹ (wielkie pozostaj¹ bez zmian)
 *
 * Zdajê sobie sprawê z istnienia funkcji 'toupper()' w bibliotece 'cstdlib'
 * That's just faster
 *
 * @param  const char ch znak do zamiany
 * @return char
 */
inline char to_upper(const char ch)
{
	return char(ch - (ch >= 'a' && ch <= 'z') * 32);
} // End to_upper()

int main()
{
	menu::cls(); // Czyszczenie ekranu jakby coœ by³o wczeœniej uruchamiane oraz dla zmiany ew. koloru t³a
	menu::print("Przyk³adowy tekst wydrukowany w konsoli i pokolorowany :)", "Przyk³adowy");
	menu::print("ÊêÓó¥¹Œœ£³¯¿ŸÆæÑñ", menu::ALL);
	menu::print("ÊêÓó¥¹Œœ£³¯¿ŸÆæÑñ", 0);
	menu::pause();
	menu::cls();
	menu::print("Inne zdanie wydrukowane w konsoli i pokolorowane :)", "zdanie");
	menu::pause();

	return 0;
}
