#include "console.h"

#ifndef _WIN32
	/// Definicje tablic z parametrami formatującymi tekst w Linuksie
	constexpr unsigned console::NORMAL_TEXT_STYLE[];
	constexpr unsigned console::INDEX_TEXT_STYLE[];
#else
	/**
	 * Pobieranie od razu jako struktura coords
	 *
	 * @return COORD koordynaty kursora
	 */
	COORD cursor::coords() {
		COORD coord;
		coord.X = this->x; coord.Y = this->y;
		return coord;
	}
#endif

/** 
 * Funkcja czyszcząca ekran
 *
 * Tworzy różny kod w zależności od systemu docelowego (Windows/Linux).
 * Nie używam 'system("cls")', gdyż może to prowadzić do problemów zw. z bezpieczeństwem.
 *
 * @return void
 */
void console::cls() {
#ifdef _WIN32
	// Czyszczenie ekranu na podstawie funkcji z oficjalnej strony Microsoftu
	// Używa WinAPI, kompilowane tylko na Windows
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console_handle, NORMAL_TEXT_STYLE); // W razie potrzeby zmiana koloru konsoli na określony
	COORD screen = { 0, 0 };
	DWORD chars_written, con_size;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(console_handle, &csbi);
	con_size = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(console_handle, ' ', con_size, screen, &chars_written);
	GetConsoleScreenBufferInfo(console_handle, &csbi);
	FillConsoleOutputAttribute(console_handle, csbi.wAttributes, con_size, screen, &chars_written);
	SetConsoleCursorPosition(console_handle, screen);
#else
	// Czyszczenie ekranu za pomocą ANSI Escape Codes na podstawie danych znalezionych w internecie
	// Używa ANSI Escape Codes, działa tylko na Linuksie i tylko tam kompilowane
	std::cout << "\033[2J\033[1;1H";
	std::rewind(stdout);
#endif
}

/**
 * Zatrzymanie konsoli (używane do efektu drukowania w menu).
 *
 * @return void
 */
void console::sleep() {
	unsigned writing_delay = 0; // Opóżnienie przy wypisywaniu liter
	if (console::FPS > 0) { // Jeśli ma być jakieś opóżnienie, liczenie go na podstawie podanego FPS
		writing_delay = 1000 / console::FPS;
	}
	else return;
	// Delay tylko przed literą/cyfrą
#ifdef _WIN32
	Sleep(writing_delay);
#else
	std::cout.flush();
	usleep(1000 * writing_delay); // Opóźnienie mniejsze, bo ustawienie stylów trwa odrobinę więcej
#endif
}

/**
 * Ustawienie stylu (np. kolor tekstu, podkreślenie, etc.)
 *
 * @param  const unsigned type typ stylu (np. zwykły tekst, zaznaczony)
 * @return void
 */
void console::set_style(const unsigned type) {
#ifdef _WIN32
	HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (type == console::NORMAL_STYLE) {
		SetConsoleTextAttribute(console_out, NORMAL_TEXT_STYLE);
	}
	else {
		SetConsoleTextAttribute(console_out, INDEX_TEXT_STYLE);
	}
#else
	printf("\e[?25l"); // Ukrycie wskaźnika kursora
	if (type == console::NORMAL_STYLE) {
		for (unsigned j = 0; j < ARRAYSIZE(console::NORMAL_TEXT_STYLE); ++j) {
			printf("%c[%dm", ESC, console::NORMAL_TEXT_STYLE[j]);
		}
	}
	else {
		for (unsigned j = 0; j < ARRAYSIZE(console::INDEX_TEXT_STYLE); ++j) {
			printf("%c[%dm", ESC, console::INDEX_TEXT_STYLE[j]);
		}
	}
#endif
}

/**
 * Drukowanie wiersza menu
 *
 * Jeśli indeks litery (liczony od 0) nie zostanie podany, nie będzie
 * zaznaczonej żadnej. Jeśli indeks zostanie podany, wyświetlona zostanie
 * litera o określonym numerze. Podanie console::ALL powoduje kolorowanie
 * całego wiersza na kolor indeksu.
 *
 * @param  const string &row wiersz do wypisania
 * @param  const unsigned key_letter = -1 litera do zaznaczenia
 * @return void
 */
void console::print(const string &row, const int key_letter) {
	/// Wstępne ustawienia konsoli - ustawienie atrybutów tekstu niekolorowanego/kolorowanego
	if (key_letter == console::ALL || key_letter == 0) {
		console::set_style(console::INDEX_STYLE);
	}
	else {
		console::set_style(console::NORMAL_STYLE);
	}

	/// Drukowanie kolejnych liter tekstu
	for (unsigned i = 0; i < row.length(); ++i)
	{
		if (i == unsigned(key_letter) && key_letter != 0) {
			console::set_style(INDEX_STYLE);
		}
		else if (i == unsigned(key_letter + 1)) {
			console::set_style(NORMAL_STYLE);
		}
		console::sleep();
		stdcout << row[i];
	} // End for
	stdcout << std::endl;
}

/**
 * Alias dla funkcji wyświetlającej menu z pokolorowanym indeksem.
 *
 * Jeśli podany zostanie wyraz to zaznaczona zostanie pierwsza litera.
 *
 * @param  const string &row wiersz do wypisania
 * @param  const string &keyword litera lub wyraz do zaznaczenia
 * @return void
 */
void console::print(const string &row, const string &keyword) {
	unsigned pos = row.find(keyword);
	if (pos != string::npos) console::print(row, pos);
	else console::print(row);
}

/**
 * Alias dla funkcji wyświetlającej menu z pokolorowanym indeksem.
 *
 * Jeśli podany zostanie wyraz to zaznaczona zostanie litera tego wyrazu
 * o indeksie podanym jako ostatni argument (liczone od 0).
 *
 * @param  const string &row wiersz do wypisania
 * @param  const string &keyword litera lub wyraz do zaznaczenia
 * @param  const unsigned letter numer litery w wyrazie do zaznaczenia
 * @return void
 */
void console::print(const string &row, const string &keyword, const unsigned letter) {
	unsigned pos = row.find(keyword);
	if (pos != std::string::npos) console::print(row, pos + letter);
	else console::print(row);
}

/**
 * Oczekiwanie na użyszkodnika aby mógł zobaczyć wynik.
 * Nie używam 'system("pause")', gdyż może to prowadzić do problemów zw. z bezpieczeństwem.
 * Poza tym, autorska funkcja pozwala na większą kontrolę nad tym, co i jak jest wypisane.
 *
 * @return void
 */
void console::pause() {
	console::print(_("\nWciśnij <ENTER> aby kontynuować..."));
	stdcin.sync(); // Kasowanie zbędnych znaków z bufora
	stdcin.get();  // Oczekiwanie na wciśnięcie klawisza
}

/**
* Funkcja przenosząca kursor na określoną pozycję w oknie konsoli (pozwala wprowadzić zmiany bez redrawa okna)
*
* @param  const unsigned x numer kolumny
* @param  const unsigned y numer wiersza
* @return void
*/
void console::gotoxy(const unsigned x, const unsigned y) {
#ifdef _WIN32
	COORD coord;
	coord.X = x; coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
	printf("\e[%d;%dH", x, y);
#endif
}

/**
* Funkcja przenosząca kursor na określoną pozycję w oknie konsoli (pozwala wprowadzić zmiany bez redrawa okna
* Alias dla funkcji console::gotoxy(x,y)
*
* @param  const unsigned x numer kolumny
* @param  const unsigned y numer wiersza
* @return void
*/
void console::set_pos(const unsigned x, const unsigned y) {
	console::gotoxy(x, y);
}

/**
 * Pobieranie pozycji kursora w konsoli
 * Aktualnie nie działa na Linuksie
 *
 * @return cursor pozycja kursora
 */
cursor console::get_cursor_pos() {
#ifdef _WIN32
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

	return cursor(info.dwCursorPosition.X, info.dwCursorPosition.Y);
#else
	// @todo: zrobienie tak, żeby działało na Linuksie
	return cursor;
#endif
}
/**
 * Ustawianie tytułu okna konsoli
 *
 * @param  const string &title tytuł okna konsoli
 * @return void
 */
string console::title(const string &title) {
	if (title == _("")) return _("Tytuł konsoli");

#ifdef _WIN32
	SetConsoleTitleW(title.c_str());
#else
	printf("\033]0;%s\007", title.c_str());
#endif

	return title;
}