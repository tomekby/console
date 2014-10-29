#include "menu.h"

/**
 * @TODO:
 *      - mo�liwo�� ustawiania tytu�u konsoli
 *      - mo�liwo�� ustawiania wielko��i konsoli
 *      - mo�liwo�� ustalania kodowania (Windows)
 *      - naprawienie kolorowania i polskich znak�w na linuksie (!)
 */

/**
 * Funkcja zamieniaj�ca liter� ma�� na wielk� (wielkie pozostaj� bez zmian)
 *
 * Zdaj� sobie spraw� z istnienia funkcji 'toupper()' w bibliotece 'cstdlib'
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
	menu::cls(); // Czyszczenie ekranu jakby co� by�o wcze�niej uruchamiane oraz dla zmiany ew. koloru t�a
	menu::print("Przyk�adowy tekst wydrukowany w konsoli i pokolorowany :)", "Przyk�adowy");
	menu::print("���󥹌�����������", menu::ALL);
	menu::print("���󥹌�����������", 0);
	menu::pause();
	menu::cls();
	menu::print("Inne zdanie wydrukowane w konsoli i pokolorowane :)", "zdanie");
	menu::pause();

	return 0;
}
