#include "menu.h"

#ifndef _WIN32
	/// Definicje tablic z parametrami formatuj¹cymi tekst w Linuksie
	constexpr unsigned menu::NORMAL_TEXT_STYLE[];
	constexpr unsigned menu::INDEX_TEXT_STYLE[];
#endif

/**
 * Funkcja czyszcz¹ca ekran
 *
 * Tworzy ró¿ny kod w zale¿noœci od systemu docelowego (Windows/Linux).
 * Nie u¿ywam 'system("cls")', gdy¿ mo¿e to prowadziæ do problemów zw. z bezpieczeñstwem.
 *
 * @return void
 */
void menu::cls() {
#ifdef _WIN32
	// Czyszczenie ekranu na podstawie funkcji z oficjalnej strony Microsoftu
	// U¿ywa WinAPI, kompilowane tylko na Windows
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, NORMAL_TEXT_STYLE); // W razie potrzeby zmiana koloru konsoli na okreœlony
	COORD screen = { 0, 0 };
	DWORD chars_written, con_size;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(console, &csbi);
	con_size = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(console, ' ', con_size, screen, &chars_written);
	GetConsoleScreenBufferInfo(console, &csbi);
	FillConsoleOutputAttribute(console, csbi.wAttributes, con_size, screen, &chars_written);
	SetConsoleCursorPosition(console, screen);
#else
	// Czyszczenie ekranu za pomoc¹ ANSI Escape Codes na podstawie danych znalezionych w internecie
	// U¿ywa ANSI Escape Codes, dzia³a tylko na Linuksie i tylko tam kompilowane
	std::cout << "\033[2J\033[1;1H";
	std::rewind(stdout);
#endif
}

/**
 * Zatrzymanie konsoli (u¿ywane do efektu drukowania w menu).
 *
 * @return void
 */
void menu::sleep() {
	unsigned writing_delay = 0; // Opó¿nienie przy wypisywaniu liter
	if (menu::FPS > 0) { // Jeœli ma byæ jakieœ opó¿nienie, liczenie go na podstawie podanego FPS
		writing_delay = 1000 / menu::FPS;
	}
	else return;
	// Delay tylko przed liter¹/cyfr¹
#ifdef _WIN32
	Sleep(writing_delay);
#else
	std::cout.flush();
	usleep(1000 * writing_delay); // OpóŸnienie mniejsze, bo ustawienie stylów trwa odrobinê wiêcej
#endif
}

/**
 * Ustawienie stylu (np. kolor tekstu, podkreœlenie, etc.)
 *
 * @param  const unsigned type typ stylu (np. zwyk³y tekst, zaznaczony)
 * @return void
 */
void menu::set_style(const unsigned type) {
#ifdef _WIN32
	HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (type == menu::NORMAL_STYLE) {
		SetConsoleTextAttribute(console_out, NORMAL_TEXT_STYLE);
	}
	else {
		SetConsoleTextAttribute(console_out, INDEX_TEXT_STYLE);
	}
#else
	printf("\e[?25l"); // Ukrycie wskaŸnika kursora
	if (type == menu::NORMAL_STYLE) {
		for (unsigned j = 0; j < ARRAYSIZE(menu::NORMAL_TEXT_STYLE); ++j) {
			printf("%c[%dm", ESC, menu::NORMAL_TEXT_STYLE[j]);
		}
	}
	else {
		for (unsigned j = 0; j < ARRAYSIZE(menu::INDEX_TEXT_STYLE); ++j) {
			printf("%c[%dm", ESC, menu::INDEX_TEXT_STYLE[j]);
		}
	}
#endif
}

/**
 * Drukowanie wiersza menu
 *
 * Jeœli indeks litery (liczony od 0) nie zostanie podany, nie bêdzie
 * zaznaczonej ¿adnej. Jeœli indeks zostanie podany, wyœwietlona zostanie
 * litera o okreœlonym numerze. Podanie menu::ALL powoduje kolorowanie
 * ca³ego wiersza na kolor indeksu.
 *
 * @param  std::string row wiersz do wypisania
 * @param  const unsigned key_letter = -1 litera do zaznaczenia
 * @return void
 */
void menu::print(std::string row, const int key_letter) {
	/// Wstêpne ustawienia konsoli - ustawienie atrybutów tekstu niekolorowanego/kolorowanego
	if (key_letter == menu::ALL || key_letter == 0) {
		menu::set_style(menu::INDEX_STYLE);
	}
	else {
		menu::set_style(menu::NORMAL_STYLE);
	}
#ifdef _WIN32
	/// Konwersja znaków na formê mo¿liw¹ do wydrukowania w konsoli
	row = menu::win1250_to_cp852(row);
#endif
	/// Drukowanie kolejnych liter tekstu
	for (unsigned i = 0; i < row.length(); ++i)
	{
		if (i == unsigned(key_letter) && key_letter != 0) {
			menu::set_style(INDEX_STYLE);
		}
		else if (i == unsigned(key_letter + 1)) {
			menu::set_style(NORMAL_STYLE);
		}
		menu::sleep();
		std::cout << row[i];
	} // End for
	std::cout << std::endl;
}

/**
 * Alias dla funkcji wyœwietlaj¹cej menu z pokolorowanym indeksem.
 *
 * Jeœli podany zostanie wyraz to zaznaczona zostanie pierwsza litera.
 *
 * @param  const std::string row wiersz do wypisania
 * @param  const std::string keyword litera lub wyraz do zaznaczenia
 * @return void
 */
void menu::print(const std::string row, const std::string keyword) {
	unsigned pos = row.find(keyword);
	if (pos != std::string::npos) menu::print(row, pos);
	else menu::print(row);
}

/**
 * Alias dla funkcji wyœwietlaj¹cej menu z pokolorowanym indeksem.
 *
 * Jeœli podany zostanie wyraz to zaznaczona zostanie litera tego wyrazu
 * o indeksie podanym jako ostatni argument (liczone od 0).
 *
 * @param  const std::string row wiersz do wypisania
 * @param  const std::string keyword litera lub wyraz do zaznaczenia
 * @param  const unsigned letter numer litery w wyrazie do zaznaczenia
 * @return void
 */
void menu::print(const std::string row, const std::string keyword, const unsigned letter) {
	unsigned pos = row.find(keyword);
	if (pos != std::string::npos) menu::print(row, pos + letter);
	else menu::print(row);
}

/**
 * Oczekiwanie na u¿yszkodnika aby móg³ zobaczyæ wynik.
 * Nie u¿ywam 'system("pause")', gdy¿ mo¿e to prowadziæ do problemów zw. z bezpieczeñstwem.
 * Poza tym, autorska funkcja pozwala na wiêksz¹ kontrolê nad tym, co i jak jest wypisane.
 *
 * @return void
 */
void menu::pause() {
	menu::print("\nWciœnij <ENTER> aby kontynuowaæ...");
	std::cin.sync(); // Kasowanie zbêdnych znaków z bufora
	std::cin.get();  // Oczekiwanie na wciœniêcie klawisza
}

#ifdef _WIN32
/**
 * Funkcja konwertuj¹ca znaki z poza ASCII z ANSI na CP 852 (kodowanie konsoli Windows)
 *
 * @param  std::string string ci¹g znaków do przetworzenia
 * @return std::string
 */
std::string menu::win1250_to_cp852(std::string string) {
	// Tablica konwersji znaków
	char conv_table[0xFF];
	for (unsigned i = 0; i < 0xFF; ++i) conv_table[i] = 0;
	conv_table[0xA5] = 0xA4; /* ¥ */ conv_table[0xC6] = 0x8F; /* Æ */ conv_table[0xCA] = 0xA8; /* Ê */ conv_table[0xA3] = 0x9D; /* £ */
	conv_table[0xD1] = 0xE3; /* Ñ */ conv_table[0xD3] = 0xE0; /* Ó */ conv_table[0x8C] = 0x97; /* Œ */ conv_table[0x8F] = 0x8D; /*  */
	conv_table[0xAF] = 0xBD; /* ¯ */ conv_table[0xB9] = 0xA5; /* ¹ */ conv_table[0xE6] = 0x86; /* æ */ conv_table[0xEA] = 0xA9; /* ê */
	conv_table[0xB3] = 0x88; /* ³ */ conv_table[0xF1] = 0xE4; /* ñ */ conv_table[0xF3] = 0xA2; /* ó */ conv_table[0x9C] = 0x98; /* œ */
	conv_table[0x9F] = 0xAB; /* Ÿ */ conv_table[0xBF] = 0xBE; /* ¿ */
	// Poprawki
	for (unsigned i = 0; i < string.length(); ++i) {
		// Interesuje nas tylko drugi (ostatni) bajt - wartoœæ char'a
		const unsigned short ch = static_cast<unsigned short>(string[i]) & 0x00FF;
		if (ch > 0x7F && ch < 0xFF && conv_table[ch] != 0) {
			string[i] = conv_table[ch];
		}
	}

	return string;
}
#endif


/**
* Funkcja przenosz¹ca kursor na okreœlon¹ pozycjê w oknie konsoli (pozwala wprowadziæ zmiany bez redrawa okna)
*
* @param  const int x numer kolumny
* @param  const int y numer wiersza
* @return void
*/
void menu::gotoxy(const unsigned x, const unsigned y) {
#ifdef _WIN32
	COORD coord;
	coord.X = x; coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
	printf("\e[%d;%dH", x, y);
#endif
}

/**
* Funkcja przenosz¹ca kursor na okreœlon¹ pozycjê w oknie konsoli (pozwala wprowadziæ zmiany bez redrawa okna
* Alias dla funkcji menu::gotoxy(x,y)
*
* @param  const int x numer kolumny
* @param  const int y numer wiersza
* @return void
*/
void menu::set_pos(const unsigned x, const unsigned y) {
	menu::gotoxy(x, y);
}
