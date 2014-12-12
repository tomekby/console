#pragma once

#include "sudoku.h"
#include "console.h"
#include <cstdint>

class list
{
	friend void time_counter(list*);
public:
	list(const short counter_x = -1, const unsigned short counter_y = 0, const short help_x = -1, const unsigned short help_y = 4)
		: COUNTER_X(counter_x), COUNTER_Y(counter_y), HELP_X(help_x), HELP_Y(help_y) {};
	const size_t size() { return _size; }
	list& draw_counter(const int value);
	inline const list& draw_counter() { return draw_counter(move_counter); };
	list& draw_help();
	inline list& operator++();
	void add(const sudoku::board board);
	void add(const sudoku::board board, const int xml_id);
	void set_as_current(const int xml_id);
	sudoku::board* get_current();
	sudoku::board* undo();
	sudoku::board* redo();
	void reset();
	console::string serialize();
private:
	void remove_unwanted();

	// Koordynaty licznika ruchów
	short COUNTER_X;
	unsigned short COUNTER_Y;
	// Koordynaty pomocy
	short HELP_X;
	unsigned short HELP_Y;
	// Tekst wyświetlany w miejscu licznika ruchów
	const console::string COUNTER_TEXT = _("Liczba ruchów: ");
	// Ilość wykonanych ruchów w grze
	int move_counter = 0;
	uint_fast64_t time_counter = 0;

	// Pojedynczy element listy dwukierunkowej
	class element {
	public:
		element(const sudoku::board &board, element *next = nullptr, element *prev = nullptr) : value(board), prev(prev), next(next) {}
		//element(const sudoku::board &board, std::shared_ptr<element> next = nullptr, std::shared_ptr<element> prev = nullptr) : element(board, prev, next) {}
		element *prev = nullptr;
		element *next = nullptr;
		// Trochę szkoda pamięci na całe stany, ale tak łatwiej dostosować do XML'a
		sudoku::board value;
		// Id przydzielone w pliku XML
		size_t xml_id = -1;
	};

	console::string serialize_element(element *el, const size_t nr);

	// Aktualnie używany element
	element *current = nullptr;
	element *head = nullptr, *tail = nullptr;
	// Rozmiar listy
	size_t _size = 0;
};

