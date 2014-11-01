#include "menu.h"

#ifndef _WIN32
	/// Definicje tablic z parametrami formatującymi tekst w Linuksie
	constexpr unsigned menu::NORMAL_TEXT_STYLE[];
	constexpr unsigned menu::INDEX_TEXT_STYLE[];
#endif

/**
 * Funkcja czyszcząca ekran
 *
 * Tworzy różny kod w zależności od systemu docelowego (Windows/Linux).
 * Nie używam 'system("cls")', gdyż może to prowadzić do problemów zw. z bezpieczeństwem.
 *
 * @return void
 */
void menu::cls() {
#ifdef _WIN32
	// Czyszczenie ekranu na podstawie funkcji z oficjalnej strony Microsoftu
	// Używa WinAPI, kompilowane tylko na Windows
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, NORMAL_TEXT_STYLE); // W razie potrzeby zmiana koloru konsoli na określony
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
	// Czyszczenie ekranu za pomocą ANSI Escape Codes na podstawie danych znalezionych w internecie
	// Używa ANSI Escape Codes, działa tylko na Linuksie i tylko tam kompilowane
	std::wcout << L"\033[2J\033[1;1H";
	std::rewind(stdout);
#endif
}

/**
 * Zatrzymanie konsoli (używane do efektu drukowania w menu).
 *
 * @return void
 */
void menu::sleep() {
	unsigned writing_delay = 0; // Opóżnienie przy wypisywaniu liter
	if (menu::FPS > 0) { // Jeśli ma być jakieś opóżnienie, liczenie go na podstawie podanego FPS
		writing_delay = 1000 / menu::FPS;
	}
	else return;
	// Delay tylko przed literą/cyfrą
#ifdef _WIN32
	Sleep(writing_delay);
#else
	std::wcout.flush();
	usleep(1000 * writing_delay); // Opóźnienie mniejsze, bo ustawienie stylów trwa odrobinę więcej
#endif
}

/**
 * Ustawienie stylu (np. kolor tekstu, podkreślenie, etc.)
 *
 * @param  const unsigned type typ stylu (np. zwykły tekst, zaznaczony)
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
	wprintf("\e[?25l"); // Ukrycie wskaźnika kursora
	if (type == menu::NORMAL_STYLE) {
		for (unsigned j = 0; j < ARRAYSIZE(menu::NORMAL_TEXT_STYLE); ++j) {
			wprintf("%c[%dm", ESC, menu::NORMAL_TEXT_STYLE[j]);
		}
	}
	else {
		for (unsigned j = 0; j < ARRAYSIZE(menu::INDEX_TEXT_STYLE); ++j) {
			wprintf("%c[%dm", ESC, menu::INDEX_TEXT_STYLE[j]);
		}
	}
#endif
}

/**
 * Drukowanie wiersza menu
 *
 * Jeśli indeks litery (liczony od 0) nie zostanie podany, nie będzie
 * zaznaczonej żadnej. Jeśli indeks zostanie podany, wyświetlona zostanie
 * litera o określonym numerze. Podanie menu::ALL powoduje kolorowanie
 * całego wiersza na kolor indeksu.
 *
 * @param  std::wstring row wiersz do wypisania
 * @param  const unsigned key_letter = -1 litera do zaznaczenia
 * @return void
 */
void menu::print(std::wstring row, const int key_letter) {
	/// Wstępne ustawienia konsoli - ustawienie atrybutów tekstu niekolorowanego/kolorowanego
	if (key_letter == menu::ALL || key_letter == 0) {
		menu::set_style(menu::INDEX_STYLE);
	}
	else {
		menu::set_style(menu::NORMAL_STYLE);
	}
#ifdef _WIN32
	/// Konwersja znaków na formę możliwą do wydrukowania w konsoli
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
		std::wcout << row[i];
	} // End for
	std::wcout << std::endl;
}

/**
 * Alias dla funkcji wyświetlającej menu z pokolorowanym indeksem.
 *
 * Jeśli podany zostanie wyraz to zaznaczona zostanie pierwsza litera.
 *
 * @param  const std::wstring row wiersz do wypisania
 * @param  const std::wstring keyword litera lub wyraz do zaznaczenia
 * @return void
 */
void menu::print(const std::wstring row, const std::wstring keyword) {
	unsigned pos = row.find(keyword);
	if (pos != std::wstring::npos) menu::print(row, pos);
	else menu::print(row);
}

/**
 * Alias dla funkcji wyświetlającej menu z pokolorowanym indeksem.
 *
 * Jeśli podany zostanie wyraz to zaznaczona zostanie litera tego wyrazu
 * o indeksie podanym jako ostatni argument (liczone od 0).
 *
 * @param  const std::wstring row wiersz do wypisania
 * @param  const std::wstring keyword litera lub wyraz do zaznaczenia
 * @param  const unsigned letter numer litery w wyrazie do zaznaczenia
 * @return void
 */
void menu::print(const std::wstring row, const std::wstring keyword, const unsigned letter) {
	unsigned pos = row.find(keyword);
	if (pos != std::wstring::npos) menu::print(row, pos + letter);
	else menu::print(row);
}

/**
 * Oczekiwanie na użyszkodnika aby mógł zobaczyć wynik.
 * Nie używam 'system("pause")', gdyż może to prowadzić do problemów zw. z bezpieczeństwem.
 * Poza tym, autorska funkcja pozwala na większą kontrolę nad tym, co i jak jest wypisane.
 *
 * @return void
 */
void menu::pause() {
	menu::print(L"\nWciśnij <ENTER> aby kontynuować...");
	std::wcin.sync(); // Kasowanie zbędnych znaków z bufora
	std::wcin.get();  // Oczekiwanie na wciśnięcie klawisza
}

#ifdef _WIN32
/**
 * Funkcja konwertująca znaki z poza ASCII z ANSI na CP 852 (kodowanie konsoli Windows)
 *
 * @param  std::wstring string ciąg znaków do przetworzenia
 * @return std::wstring
 */
std::wstring menu::win1250_to_cp852(std::wstring string) {
	//// Tablica konwersji znaków
	//char conv_table[0xFF];
	//for (unsigned i = 0; i < 0xFF; ++i) conv_table[i] = 0;
	//conv_table[0xA5] = 0xA4; /* Ą */ conv_table[0xC6] = 0x8F; /* Ć */ conv_table[0xCA] = 0xA8; /* Ę */ conv_table[0xA3] = 0x9D; /* Ł */
	//conv_table[0xD1] = 0xE3; /* Ń */ conv_table[0xD3] = 0xE0; /* Ó */ conv_table[0x8C] = 0x97; /* Ś */ conv_table[0x8F] = 0x8D; /* Ź */
	//conv_table[0xAF] = 0xBD; /* Ż */ conv_table[0xB9] = 0xA5; /* ą */ conv_table[0xE6] = 0x86; /* ć */ conv_table[0xEA] = 0xA9; /* ę */
	//conv_table[0xB3] = 0x88; /* ł */ conv_table[0xF1] = 0xE4; /* ń */ conv_table[0xF3] = 0xA2; /* ó */ conv_table[0x9C] = 0x98; /* ś */
	//conv_table[0x9F] = 0xAB; /* ź */ conv_table[0xBF] = 0xBE; /* ż */
	//// Poprawki
	//for (unsigned i = 0; i < string.length(); ++i) {
	//	// Interesuje nas tylko drugi (ostatni) bajt - wartość char'a
	//	const unsigned short ch = static_cast<unsigned short>(string[i]) & 0x00FF;
	//	if (ch > 0x7F && ch < 0xFF && conv_table[ch] != 0) {
	//		string[i] = conv_table[ch];
	//	}
	//}

	return string;
}
#endif


/**
* Funkcja przenosząca kursor na określoną pozycję w oknie konsoli (pozwala wprowadzić zmiany bez redrawa okna)
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
	wprintf("\e[%d;%dH", x, y);
#endif
}

/**
* Funkcja przenosząca kursor na określoną pozycję w oknie konsoli (pozwala wprowadzić zmiany bez redrawa okna
* Alias dla funkcji menu::gotoxy(x,y)
*
* @param  const int x numer kolumny
* @param  const int y numer wiersza
* @return void
*/
void menu::set_pos(const unsigned x, const unsigned y) {
	menu::gotoxy(x, y);
}
