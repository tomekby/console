/**
* Klasa odpowiedzialna za obsługę frontendu, w szczególności menu
* Kompilowany kod jest różny w zależności od docelowego systemu.
* Jest to spowodowane tym, że Windows i Linux używają różnych
* sposobów formatowania konsoli (WinAPI/ANSI Escape Codes).
*
* @author     Tomasz Stasiak
* @copyright  (c) 2012/2014 Tomasz Stasiak
*/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string>

namespace console {

#ifdef _WIN32
#   define _WIN32_WINNT 0x0501
#   include <windows.h>
#	include <io.h>
#	include <fcntl.h>
#	include <tchar.h>
#	define _(x) _T(x)
#	define _out std::wcout
#	define _in  std::wcin
	typedef std::wstring string;
#	define LASTMODE		-1
#	define C80			 3
#	define C4350		 8
#	define FULLSCREEN	 0x8000

#else
#   include <cstdio>
#   include <unistd.h>
#	define _(x) x
#	define _out std::cout
#	define _in  std::cin
#	define ARRAYSIZE(x) sizeof(x) / sizeof(x[0])
	typedef std::string string;
#endif

	/**
	 * Ustawienia
	 */
#ifndef _WIN32
	/// ASCII escape codes używane do formatowania konsoli
	const char ESC = 0x1B; // Escape (musi poprzedzać escape code'y)
	const char RESET = 0x00; // Resetowanie ustawień
	const char BRIGHT = 0x01; // Jaskrawsze kolory
	const char UNDERLINE = 0x04; // Podkreślenie
	// Kolory tekstu w konsoli
	enum{ BLACK = 0x1E, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };
	// Kolory tła w konsoli
	enum{ BG_BLACK = 0x28, BG_RED, BG_GREEN, BG_YELLOW, BG_BLUE, BG_MAGENTA, BG_CYAN, BG_WHITE };
#else
	// Ustawienie kodowania w konsoli windows
	// Hack umożliwiający init jeszcze przed main()
	const auto _SET_UNICODE = _setmode(_fileno(stdout), _O_U8TEXT);
#endif

	/**
	 * Dane kursora
	 */
	class cursor {
	public:
		cursor() : x(0), y(0) {};
		cursor(unsigned x, unsigned y) : x(x), y(y) {};
#ifdef _WIN32
		COORD coords();
#endif
		unsigned x, y;
	};

#ifdef _WIN32
	/**
	 * Struktura skopiowana z szablonu do projektu
	 * W sumie nie wszystkie pola są potrzebne, ale 
	 * nie chce mi się sprawdzać co można wyrzucić
	 */
	struct _Conio2ThreadData {
		//int attrib;
		int charCount;
		int charValue;
		int charFlag;
		int ungetCount;
		int ungetBuf[16];
		HANDLE input;
		int _wscroll;
		int width;
		int height;
		int origwidth;
		int origheight;
		int origdepth;
		int lastmode;
		HANDLE output;
		DWORD prevOutputMode;
		DWORD prevInputMode;
	};
	bool _InitConio2(); // Funkcja inicjująca w/w strukturę
	// Hack umożliwiający załadowanie przed main
	// Nie wymaga używania makr do kontroli gdzie ląduje kod
	const auto _INIT_HACK = _InitConio2();
#endif

	/**
	 * Główna obsługa konsoli
	 */
	void cls();
	void pause();
	void print(const string &row, const int key_letter = -1);
	void print(const string &row, const string &keyword);
	void print(const string &row, const string &keyword, const unsigned letter);
	// Wszystkie znaki mają być pokolorowane
	const unsigned ALL = 0xDEADBEEF;
	void gotoxy(const unsigned x, const unsigned y);
	void gotoxy(const cursor &pos);
	string title(const string &title);
	cursor get_cursor_pos();
	int getch();
	bool _HandleKeyEvent(INPUT_RECORD *buf);
	void _sleep();
	void _set_style(const unsigned type);
	/// Ilość znaków wypisywanych w ciągu sekundy
	const unsigned _FPS = 0;
	/// Typ ustawianych informacji
	const unsigned _NORMAL_STYLE = 1;
	const unsigned _INDEX_STYLE = 2;
#ifdef _WIN32
	/// Style: http://msdn.microsoft.com/en-us/library/windows/desktop/ms682088%28v=vs.85%29.aspx#_win32_character_attributes
	// Styl normalnego wypisywanego tekstu
	const unsigned _NORMAL_TEXT_STYLE = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	// Styl indeksu (zaznaczonej litery), który trzeba wpisać aby wybrać element z menu
	const unsigned _INDEX_TEXT_STYLE = FOREGROUND_RED | FOREGROUND_INTENSITY;
#else
	/// Style widoczne wyżej
	constexpr unsigned _NORMAL_TEXT_STYLE[] = { RESET, BG_BLACK, BRIGHT, GREEN };
	constexpr unsigned _INDEX_TEXT_STYLE[] = { RESET, BG_BLACK, UNDERLINE, BRIGHT, RED };
#endif
} // End namespace console

#endif // CONSOLE_H
