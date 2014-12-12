#include "list.h"

/**
 * Rysowanie licznika ruchów
 *
 * @param  value wartość licznika
 * @return *this;
 */
list& list::draw_counter(const int value) {
	const console::cursor TMP = console::get_cursor_pos();
	const console::string PRINTED = COUNTER_TEXT + std::to_wstring(value);
	// Miejsce licznika
	const unsigned POS_X = COUNTER_X < 0 ? console::get_size().first - PRINTED.length() - 1 : COUNTER_X;
	console::gotoxy(POS_X, COUNTER_Y);
	// Wypisanie wartości licznika
	console::print(PRINTED, console::ALL);

	// Powrót do poprzedniej pozycji kursora
	console::gotoxy(TMP);
	return *this;
}

/**
 * Rysowanie pomocy
 *
 * @return *this
 */
list& list::draw_help() {
	const auto POS = console::get_cursor_pos();

	// Komunikaty pomocy
	const console::string info[] = {
		_("Pomoc do gry:"),
		_("←↑↓→ - poruszanie po planszy"),
		_("1..9 - wstawienie/nadpisanie liczby"),
		_("u - cofnięcie ruchu"),
		_("r - ponowienie ruchu"),
		_("l - lista możliwych cyfr w polu"),
		_("p - podpowiedź"),
		_("o - otwarcie pliku XML"),
		_("s - zapis pliku XML"),
		_("k - tryb edycji komentarza"),
	};
	// Pozycja ustalana automatycznie
	if (HELP_X == -1) {
		HELP_X = 0; // Zapobieganie int to unsigned comparison
		// Szukanie najdłuższego stringa
		for (size_t i = 0; i < 8; ++i)
			HELP_X = HELP_X < static_cast<short>(info[i].length()) ? static_cast<short>(info[i].length()) : HELP_X;
	}
	// Wypisywanie pomocy
	for (size_t i = 0; i < 10; ++i) {
		console::gotoxy(console::get_size().first - HELP_X - 1, HELP_Y + i);
		console::print(info[i], console::ALTERNATIVE);
	}
	console::gotoxy(POS);

	return *this;
}

/**
 * Inkrementacja licznika ruchów
 * Zwiększa licznik oraz wyświetla zmieniony
 * 
 * @return *this
 */
inline list& list::operator++() {
	return draw_counter(++move_counter);
}

void list::remove_unwanted() {
	// Kasowanie nieaktualnych stanów
	while (current->prev != nullptr) {
		element *prev = current->prev->prev;
		// Czyszczenie pamięci
		delete current->prev;
		current->prev = prev;
		--_size;
	}
}

/**
 * Dodawanie elementu do listy undo-redo
 *
 * @param  board obiekt zawierający planszę do dodania
 * @param  inc_counter czy licznik ma zostać zainkrementowany
 * @return void
 */
void list::add(const sudoku::board board) {
	// Jeśli lista jest pusta
	if (current == nullptr) {
		current = new element(board);
		head = tail = current;
		draw_counter(move_counter);
	}
	// Lista nie jest pusta
	else {
		// Kasowanie nieaktualnych ruchów z historii
		remove_unwanted();
		// Dodanie kolejnego elementu
		current->prev = new element(board, current);
		current = current->prev;
		head = current;
		++(*this);
	}
	++_size;
}

/**
 * Funkcja umożliwiająca dodawanie elementów undo/redo z XML'a
 *
 * @param  board obiekt zawierający planszę do dodania
 * @param  xml_id ID stanu wg. XML'a
 * @return void
 */
void list::add(const sudoku::board board, const int xml_id) {
	add(board);
	current->xml_id = xml_id;
}

/**
 * Ustawianie wskaźnika do aktualnego elementu na stan o określonym id z XML'a
 * Jeśli nie znajdzie określonego stanu nie dzieje się nic
 * 
 * @param xml_id ID stanu z pliku XML
 */
void list::set_as_current(const int xml_id) {
	for (element *el = head; el != nullptr; el = el->next) {
		if (el->xml_id == xml_id) {
			current = el;
			return;
		}
	}
}

/**
 * Czyszczenie listy undo-redo oraz liczników
 *
 * @return void
 */
void list::reset() {
	for (element *el = head; el != nullptr;) {
		element *next = el->next;
		delete el;
		el = next;
	}
	current = head = tail = nullptr;
	_size = time_counter = move_counter = 0;
}

/**
 * Cofanie ruchu
 *
 * @return wskaźnik na planszę po odrysowaniu
 */
sudoku::board* list::undo() {
	if (current->next == nullptr)
		return &(current->value);

	current = current->next;
	auto res = &(current->value.redraw_all(false));
	++(*this);

	return res;
}

/**
 * Pobieranie aktualnego stanu z listy undo-redo
 */
sudoku::board* list::get_current() {
	return &(current->value);
}

/**
 * Ponawianie ruchu z listy undo-redo
 *
 * @return wskaźnik na planszę po odrysowaniu
 */
sudoku::board* list::redo() {
	if (current->prev == nullptr)
		return &(current->value);

	current = current->prev;
	auto res = &(current->value.redraw_all(false));
	++(*this);

	return res;
}

/**
 * Serializacja określonego stanu
 *
 * @param  el wskaźnik na element do serializacji
 * @param  nr nr elementu
 * @return zserializowana plansza sudoku
 */
console::string list::serialize_element(element *el, const size_t nr) {
	// Otwarcie węzła
	console::string s = _(R"xml(<state nr=")xml");
	s += std::to_wstring(nr) + _("\">");
	// Serializacja planszy
	s += _("<board>");
	for (size_t i = 0; i < 9; ++i) {
		for (size_t j = 0; j < 9; ++j) {
			const short val = el->value.get_num_value(j, i);
			s += ((val != -1) ? std::to_wstring(val) : _("-")) + _(" ");
		}
	}
	s += _("</board>");
	// Serializacja komentarzy
	// Dokumentacja XML'a nie wyklucza pustej sekcji <comments>
	s += _("<comments>");
	for (size_t i = 0; i < 9; ++i) {
		for (size_t j = 0; j < 9; ++j) {
			const auto val = el->value.get_comments(i, j);
			// Jeśli nie ma komentarzy, pomijamy
			if (val.second == 0) continue;
			// Właściwe wpisanie komentarzy
			s += _("<comment row=\"") + std::to_wstring(j + 1) + _("\" col=\"") + std::to_wstring(i + 1) + _("\">");
			for (size_t k = 0; k < val.second; ++k) {
				s += std::to_wstring(val.first[k]) + _(" ");
			}
			s += _("</comment>");
		}
	}
	s += _("</comments></state>");

	return s;
}

/**
 * Serializacja całej listy undo-redo
 *
 * @return zserializowana lista undo-redo
 */
console::string list::serialize() {
	size_t nr = 0, current = 0;
	console::string buffer;
	const console::string start = _(R"xml(<?xml version="1.0"?><sudoku active-state=")xml");

	for (element *el = tail; el != nullptr; el = el->prev) {
		buffer += serialize_element(el, ++nr);
		// Szukanie aktualnego elementu
		if (el == this->current) {
			current = nr;
		}
	}

	// Dopisanie początku i końca
	buffer = start + std::to_wstring(current) + _("\">") + buffer;
	buffer += _(R"xml(</sudoku>)xml");

	return buffer;
}