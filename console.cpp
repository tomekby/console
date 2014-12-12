#include "console.h"

namespace console {

#ifdef _WIN32
	std::wistream& _in = std::wcin;
	std::wostream& _out = std::wcout;

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
	// Struktura konfiguracyjna dla funkcji z udostępnionego szablonu
	_Conio2ThreadData _thData;

	/**
	 * Inicjalizacja kodu z udostępnionego szablonu
	 */
	bool _InitConio2() {
		_thData.output = GetStdHandle(STD_OUTPUT_HANDLE);
		_thData.input = GetStdHandle(STD_INPUT_HANDLE);
		_thData.ungetCount = _thData.charCount = 0;

		_thData.lastmode = C80;
		SetConsoleMode(_thData.input, ENABLE_PROCESSED_INPUT);

		return true;
	}
#else
	std::istream& _in = std::cin;
	std::ostream& _out = std::cout;
#endif

	/**
	 * Wczytywanie wartości z stdin
	 */
	template<class T> T read() {
		T res;
		_in >> res;

		return res;
	}

	// Fix na linkowanie
	template string read<string>();
	template unsigned read<unsigned>();
	template unsigned short read<unsigned short>();
	template short read<short>();
	template int read<int>();

	/**
	 * Funkcja czyszcząca ekran
	 *
	 * Tworzy różny kod w zależności od systemu docelowego (Windows/Linux).
	 * Nie używam 'system("cls")', gdyż może to prowadzić do problemów zw. z bezpieczeństwem.
	 *
	 * @return void
	 */
	void cls() {
#ifdef _WIN32
		// Czyszczenie ekranu na podstawie funkcji z oficjalnej strony Microsoftu
		// Używa WinAPI, kompilowane tylko na Windows
		HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(console_handle, _NORMAL_TEXT_STYLE); // W razie potrzeby zmiana koloru konsoli na określony
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
		_out << "\033[2J\033[1;1H";
		std::rewind(stdout);
#endif
	}

	/**
	 * Zatrzymanie konsoli (używane do efektu drukowania tekstu).
	 *
	 * @return void
	 */
	void _sleep() {
		unsigned writing_delay = 0; // Opóżnienie przy wypisywaniu liter
		if (console::_FPS == 0) return;
		// Jeśli ma być jakieś opóżnienie, liczenie go na podstawie podanego FPS
		// Ciekawe optymalizacje... Jeśli _FPS == 0 to poniższy kod się nie wykona
		// Ale VS i tak się czepia, że będzie dzielenie przez 0...
		writing_delay = 1000 / (console::_FPS == 0 ? 1 : console::_FPS);
		// Delay tylko przed literą/cyfrą
#ifdef _WIN32
		Sleep(writing_delay);
#else
		_out.flush();
		usleep(1000 * writing_delay); // Opóźnienie mniejsze, bo ustawienie stylów trwa odrobinę więcej
#endif
	}

	/**
	 * Ustawienie stylu (np. kolor tekstu, podkreślenie, etc.)
	 *
	 * @param  const unsigned type typ stylu (np. zwykły tekst, zaznaczony)
	 * @return void
	 */
	void _set_style(const unsigned type) {
#ifdef _WIN32
		HANDLE console_out = GetStdHandle(STD_OUTPUT_HANDLE);
		if (type == _NORMAL_STYLE) {
			SetConsoleTextAttribute(console_out, _NORMAL_TEXT_STYLE);
		}
		else if (type == _ALTERNATIVE_STYLE) {
			SetConsoleTextAttribute(console_out, _ALTERNATIVE_TEXT_STYLE);
		}
		else {
			SetConsoleTextAttribute(console_out, _INDEX_TEXT_STYLE);
		}
#else
		printf("\e[?25l"); // Ukrycie wskaźnika kursora
		if (type == _NORMAL_STYLE) {
			for (unsigned j = 0; j < ARRAYSIZE(_NORMAL_TEXT_STYLE); ++j) {
				printf("%c[%dm", ESC, _NORMAL_TEXT_STYLE[j]);
			}
		}
		else {
			for (unsigned j = 0; j < ARRAYSIZE(_INDEX_TEXT_STYLE); ++j) {
				printf("%c[%dm", ESC, _INDEX_TEXT_STYLE[j]);
			}
		}
#endif
	}

	/**
	 * Drukowanie wiersza tekstu
	 * @todo: flatten
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
	void print(const string &row, const int key_letter) {
		/// Wstępne ustawienia konsoli - ustawienie atrybutów tekstu niekolorowanego/kolorowanego
		if (key_letter == ALL || key_letter == 0) {
			_set_style(_INDEX_STYLE);
		}
		else if (key_letter == ALTERNATIVE) {
			_set_style(_ALTERNATIVE_STYLE);
		}
		else {
			_set_style(_NORMAL_STYLE);
		}

		/// Drukowanie kolejnych liter tekstu
		for (unsigned i = 0; i < row.length(); ++i)
		{
			if (i == unsigned(key_letter) && key_letter != 0) {
				_set_style(_INDEX_STYLE);
			}
			else if (key_letter == ALTERNATIVE) {
				_set_style(_ALTERNATIVE_STYLE);
			}
			else if (i == unsigned(key_letter + 1)) {
				_set_style(_NORMAL_STYLE);
			}
			_sleep();
			_out << row[i];
		} // End for
		_out << std::endl;
	}

	/**
	 * Alias dla funkcji wyświetlającej tekst z pokolorowanym indeksem.
	 *
	 * Jeśli podany zostanie wyraz to zaznaczona zostanie pierwsza litera.
	 *
	 * @param  const string &row wiersz do wypisania
	 * @param  const string &keyword litera lub wyraz do zaznaczenia
	 * @return void
	 */
	void print(const string &row, const string &keyword) {
		const unsigned pos = row.find(keyword);
		if (pos != string::npos) print(row, pos);
		else print(row);
	}

	/**
	 * Alias dla funkcji wyświetlającej tekst z pokolorowanym indeksem.
	 *
	 * Jeśli podany zostanie wyraz to zaznaczona zostanie litera tego wyrazu
	 * o indeksie podanym jako ostatni argument (liczone od 0).
	 *
	 * @param  const string &row wiersz do wypisania
	 * @param  const string &keyword litera lub wyraz do zaznaczenia
	 * @param  const unsigned letter numer litery w wyrazie do zaznaczenia
	 * @return void
	 */
	void print(const string &row, const string &keyword, const unsigned letter) {
		const unsigned pos = row.find(keyword);
		if (pos != std::string::npos) print(row, pos + letter);
		else print(row);
	}

	/**
	 * Oczekiwanie na użyszkodnika aby mógł zobaczyć wynik.
	 * Nie używam 'system("pause")', gdyż może to prowadzić do problemów zw. z bezpieczeństwem.
	 * Poza tym, autorska funkcja pozwala na większą kontrolę nad tym, co i jak jest wypisane.
	 *
	 * @return void
	 */
	void pause() {
		print(_("\nWciśnij <ENTER> aby kontynuować..."));
		_in.sync(); // Kasowanie zbędnych znaków z bufora
		_in.get();  // Oczekiwanie na wciśnięcie klawisza
	}

	/**
	* Funkcja przenosząca kursor na określoną pozycję w oknie konsoli (pozwala wprowadzić zmiany bez redrawa okna)
	*
	* @param  const unsigned x numer kolumny
	* @param  const unsigned y numer wiersza
	* @return void
	*/
	void gotoxy(const unsigned x, const unsigned y) {
#ifdef _WIN32
		COORD coord;
		coord.X = x; coord.Y = y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
		printf("\e[%d;%dH", x, y);
#endif
	}

	/**
	* Funkcja przenosząca kursor na określoną pozycję w oknie konsoli (pozwala wprowadzić zmiany bez redrawa okna)
	* Alias dla funkcji gotoxy(x,y)
	*
	* @param  const cursor &pos pozycja kursora
	* @return void
	*/
	void gotoxy(const cursor &pos) {
		gotoxy(pos.x, pos.y);
	}

	/**
	 * Pobieranie pozycji kursora w konsoli
	 * ! Aktualnie nie działa na Linuksie
	 *
	 * @return cursor pozycja kursora
	 */
	cursor get_cursor_pos() {
#ifdef _WIN32
		CONSOLE_SCREEN_BUFFER_INFO info;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

		return cursor(info.dwCursorPosition.X, info.dwCursorPosition.Y);
#else
		// @todo: zrobienie tak, żeby działało na Linuksie
		return cursor();
#endif
	}

	/**
	 * Ustawianie tytułu okna konsoli
	 *
	 * @param  const string &title tytuł okna konsoli
	 * @return void
	 */
	string title(const string &title) {
		if (title == _("")) return _("Tytuł konsoli"); // @todo zrobienie gettera (?)

#ifdef _WIN32
		SetConsoleTitleW(title.c_str());
#else
		printf("\033]0;%s\007", title.c_str());
#endif

		return title;
	}


	/**
	 * Funkcja skopiowana z szablonu
	 */
	bool _HandleKeyEvent(INPUT_RECORD *buf) {
		int ch;
		ch = (int)(buf->Event.KeyEvent.uChar.AsciiChar) & 255;
		if (ch == 0) ch = 0x8000 + buf->Event.KeyEvent.wVirtualKeyCode;
		if (ch == 0x8010 || ch == 0x8011 || ch == 0x8012 || ch == 0x8014
			|| ch == 0x8090 || ch == 0x8091) return false;
		_thData.charCount = buf->Event.KeyEvent.wRepeatCount;
		_thData.charFlag = ch & 0x8000 ? 1 : 0;
		if (_thData.charFlag) _thData.charCount *= 2;
		switch (ch) {
		case 0x8000 + 33:	ch = 0x8000 + 73; break;
		case 0x8000 + 34:	ch = 0x8000 + 81; break;
		case 0x8000 + 35:	ch = 0x8000 + 79; break;
		case 0x8000 + 36:	ch = 0x8000 + 71; break;
		case 0x8000 + 37:	ch = 0x8000 + 75; break;
		case 0x8000 + 38:	ch = 0x8000 + 72; break;
		case 0x8000 + 39:	ch = 0x8000 + 77; break;
		case 0x8000 + 40:	ch = 0x8000 + 80; break;
		case 0x8000 + 46:	ch = 0x8000 + 83; break;
		case 0x8000 + 112:	ch = 0x8000 + 59; break;
		case 0x8000 + 113:	ch = 0x8000 + 60; break;
		case 0x8000 + 114:	ch = 0x8000 + 61; break;
		case 0x8000 + 115:	ch = 0x8000 + 62; break;
		case 0x8000 + 116:	ch = 0x8000 + 63; break;
		case 0x8000 + 117:	ch = 0x8000 + 64; break;
		case 0x8000 + 118:	ch = 0x8000 + 65; break;
		case 0x8000 + 119:	ch = 0x8000 + 66; break;
		case 0x8000 + 120:	ch = 0x8000 + 67; break;
		case 0x8000 + 121:	ch = 0x8000 + 68; break;
		case 0x8000 + 122:	ch = 0x8000 + 133; break;
		case 0x8000 + 123:	ch = 0x8000 + 134; break;
		};
		_thData.charValue = ch & 0x7fff;
		// TODO: translate proper keys (eg. arrows) to 0, xxx
		return true;
	};

	/**
	 * Funkcja skopiowana z szablonu
	 */
	int getch() {
		BOOL rv;
		DWORD n;
		INPUT_RECORD buf;

		if (_thData.ungetCount > 0) {
			_thData.ungetCount--;
			return _thData.ungetBuf[_thData.ungetCount];
		};

		if (_thData.charCount > 0) {
			_thData.charCount--;
			if (_thData.charCount & 1 && _thData.charFlag) return 0;
			else return _thData.charValue;
		};

		while (true) {
			rv = ReadConsoleInput(_thData.input, &buf, 1, &n);
			if (rv == false) continue;
			if (buf.EventType != KEY_EVENT) continue;
			if (buf.Event.KeyEvent.bKeyDown == false) continue;
			if (_HandleKeyEvent(&buf)) break;
		};

		_thData.charCount--;
		if (_thData.charCount & 1 && _thData.charFlag) return 0;
		else return _thData.charValue;
	}

	/**
	 * Pobieranie znaku i wypisanie go na konsolę
	 *
	 * @return pobrany znak
	 */
	int getche() {
		const char ch = getch();
		_out << ch;
		return ch;
	}

	/**
	 * Wymiary okna konsoli
	 *
	 * @return  <cols, rows>
	 */
	pair<unsigned, unsigned> get_size() {
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		int columns, rows;

		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

		return pair<unsigned, unsigned>(columns, rows);
	}

	/**
	 * Czyszczenie linii
	 *
	 * @param y numer linii do wyczyszczenia
	 */
	void clear_line(const unsigned y) {
		const auto size = get_size();
		if (y > size.second) return;

		// Poprzednia pozycja kursora
		const console::cursor TMP = get_cursor_pos();
		// Tworzenie stringa o szerokości okna konsoli
		string line;
		line.append(size.first, _(' '));
		// Czyszczenie
		gotoxy(0,y);
		print(line);
		// Powrót
		gotoxy(TMP);
	}

	/**
	 * Ukrywanie kursora w konsoli
	 *
	 * @return true
	 */
	bool _hide_cursor() {
		HANDLE hConsoleOutput;
		CONSOLE_CURSOR_INFO structCursorInfo;
		hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleCursorInfo(hConsoleOutput, &structCursorInfo);
		structCursorInfo.bVisible = FALSE;
		SetConsoleCursorInfo(hConsoleOutput, &structCursorInfo);

		return true;
	}
} // End namespace console