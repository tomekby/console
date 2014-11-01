/**
* Klasa odpowiedzialna za obsługę frontendu, w szczególności menu
* Kompilowany kod jest różny w zależności od docelowego systemu.
* Jest to spowodowane tym, że Windows i Linux używają różnych
* sposobów formatowania konsoli (WinAPI/ANSI Escape Codes).
*
* @author     Tomasz Stasiak
* @copyright  (c) 2012 Tomasz Stasiak
*/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <iostream>
#include <string>

#ifdef _WIN32
#   define _WIN32_WINNT 0x0501
#   include <windows.h>
#	include <io.h>
#	include <fcntl.h>
#	include <tchar.h>
#	define _(x) _T(x)
#	define stdcout std::wcout
#	define stdcin std::wcin
	typedef std::wstring string;
#else
#   include <cstdio>
#   include <unistd.h>
#	define _(x) x
#	define stdcout std::cout
#	define stdcin std::cin
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
	const auto SET_UNICODE = _setmode(_fileno(stdout), _O_U16TEXT);
#endif

class cursor {
public:
	cursor() : x(0), y(0) {};
	cursor(unsigned x, unsigned y) : x(x), y(y) {};
#ifdef _WIN32
	COORD coords();
#endif
	unsigned x, y;
};

class console {
public:
	static void cls();
	static void pause();
	static void print(const string &row, const int key_letter = -1);
	static void print(const string &row, const string &keyword);
	static void print(const string &row, const string &keyword, const unsigned letter);
	static void gotoxy(const unsigned x, const unsigned y);
	static void set_pos(const unsigned x, const unsigned y); // Alias dla powyższego
	static string title(const string &title);
	static const unsigned ALL = 0xDEADBEEF;
	static cursor get_cursor_pos();
protected:
private:
	static void sleep();
	static void set_style(const unsigned type);
	/// Ilość znaków wypisywanych w ciągu sekundy
	static const unsigned FPS = 60;
	/// Typ ustawianych informacji
	static const unsigned NORMAL_STYLE = 1;
	static const unsigned INDEX_STYLE = 2;
#ifdef _WIN32
	/// Style: http://msdn.microsoft.com/en-us/library/windows/desktop/ms682088%28v=vs.85%29.aspx#_win32_character_attributes
	// Styl normalnego wypisywanego tekstu
	static const unsigned NORMAL_TEXT_STYLE = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	// Styl indeksu (zaznaczonej litery), który trzeba wpisać aby wybrać element z menu
	static const unsigned INDEX_TEXT_STYLE = FOREGROUND_RED | FOREGROUND_INTENSITY;
#else
	/// Style widoczne wyżej
	static constexpr unsigned NORMAL_TEXT_STYLE[] = { RESET, BG_BLACK, BRIGHT, GREEN };
	static constexpr unsigned INDEX_TEXT_STYLE[] = { RESET, BG_BLACK, UNDERLINE, BRIGHT, RED };
#endif
};

#endif // CONSOLE_H
