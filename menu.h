/**
* Klasa odpowiedzialna za obs³ugê frontendu, w szczególnoœci menu
* Kompilowany kod jest ró¿ny w zale¿noœci od docelowego systemu.
* Jest to spowodowane tym, ¿e Windows i Linux u¿ywaj¹ ró¿nych
* sposobów formatowania konsoli (WinAPI/ANSI Escape Codes).
*
* @author     Tomasz Stasiak
* @copyright  (c) 2012 Tomasz Stasiak
*/
#ifndef MENU_H
#define MENU_H

#ifdef _WIN32
#   define _WIN32_WINNT 0x0501
#   include <windows.h>
#else
#   include <cstdio>
#   include <unistd.h>
#endif
#include <iostream>
#include <string>
#include <array>
#include <map>

#ifndef _WIN32
/// ASCII escape codes u¿ywane do formatowania konsoli
const char ESC = 0x1B; // Escape (musi poprzedzaæ escape code'y)
const char RESET = 0x00; // Resetowanie ustawieñ
const char BRIGHT = 0x01; // Jaskrawsze kolory
const char UNDERLINE = 0x04; // Podkreœlenie
// Kolory tekstu w konsoli
enum{ BLACK = 0x1E, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE };
// Kolory t³a w konsoli
enum{ BG_BLACK = 0x28, BG_RED, BG_GREEN, BG_YELLOW, BG_BLUE, BG_MAGENTA, BG_CYAN, BG_WHITE };
#define ARRAYSIZE(x) sizeof(x) / sizeof(x[0])
#endif

class menu
{
public:
	static void cls();
	static void pause();
	static void print(std::string row, const int key_letter = -1);
	static void print(const std::string row, const std::string keyword);
	static void print(const std::string row, const std::string keyword, const unsigned letter);
	static void gotoxy(const unsigned x, const unsigned y);
	static void set_pos(const unsigned x, const unsigned y); // Alias dla powy¿szego
	static const unsigned ALL = 0xDEADBEEF;
protected:
private:
	static void sleep();
	static void set_style(const unsigned type);
	/// Iloœæ znaków wypisywanych w ci¹gu sekundy
	static const unsigned FPS = 36;
	/// Typ ustawianych informacji
	static const unsigned NORMAL_STYLE = 1;
	static const unsigned INDEX_STYLE = 2;
#ifdef _WIN32
	static std::string win1250_to_cp852(std::string string);
	/// Style: http://msdn.microsoft.com/en-us/library/windows/desktop/ms682088%28v=vs.85%29.aspx#_win32_character_attributes
	// Styl normalnego wypisywanego tekstu
	static const unsigned NORMAL_TEXT_STYLE = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	// Styl indeksu (zaznaczonej litery), który trzeba wpisaæ aby wybraæ element z menu
	static const unsigned INDEX_TEXT_STYLE = FOREGROUND_RED | FOREGROUND_INTENSITY;
#else
	/// Style widoczne wy¿ej
	static constexpr unsigned NORMAL_TEXT_STYLE[] = { RESET, BG_BLACK, BRIGHT, GREEN };
	static constexpr unsigned INDEX_TEXT_STYLE[] = { RESET, BG_BLACK, UNDERLINE, BRIGHT, RED };
#endif
};

#endif // MENU_H
