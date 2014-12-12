#include <thread> // Musi być jako pierwszy nagłówek O.o
#include "console.h"
#ifdef _DEBUG // Dostajemy OutputDebugString umożliwiające pisanie do debuggera
#	include <..\um\debugapi.h>
#else
#	define OutputDebugString(x) 
#endif
#include "sudoku.h"
#include "list.h"
#include <chrono>
#include <cstring>
#include <mutex>
#include "XML.h"
#include "utils.h"

// Mutex dla timera
std::mutex _time_counter_mutex;
// Wskaźniki dla timera czy może startować lub kończyć działanie
// @todo przekazanie przez referencję do wątku
bool _stop_counter = false, _start_counter = false;

/**
 * Wypisywanie czasu
 *
 * @param  time czas w sekundach do sformatowania
 * @return string w postaci xh ymin zs
 */
console::string pretty_print_time(uint_fast64_t time) {
	console::string pretty_time;
	if (time / 3600) {
		pretty_time += _(" ") + std::to_wstring(time / 3600) + _("h");
		time %= 3600;
	}
	if (time / 60) {
		pretty_time += _(" ") + std::to_wstring(time / 60) + _("min");
		time %= 60;
	}
	if (time != 0 || ! pretty_time.length()) {
		pretty_time += _(" ") + std::to_wstring(time) + _("s");
	}

	return pretty_time;
}

/**
 * @todo: jakieś sensowniejsze ustawienie tego/wylot do listy (?)
 * ZDECYDOWANIE ZA DUŻE ZAGNIEŻDZENIE i za długa funkcja
 */
void time_counter(list *undo_redo) {
	const console::string COUNTER_TEXT = _("Czas od rozpoczęcia gry:");
	const uint_fast8_t INTERVAL = 1;
	auto last_check = std::chrono::steady_clock::now();
	// Oczekiwanie na sygnał pozwalający wystartować wątek
	while ( ! _start_counter) {
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	unsigned longest_text = 0;
	// Zmiana timer'a
	do {
		// Tworzenie scope'u dla mutex'a
		{
			std::lock_guard<std::mutex> lock(_time_counter_mutex);
			const console::cursor TMP = console::get_cursor_pos();
			auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - last_check).count();
			last_check = std::chrono::steady_clock::now();

			// Czas od startu jako string
			console::string PRINTED = COUNTER_TEXT + pretty_print_time(undo_redo->time_counter += elapsed);
			if (longest_text > PRINTED.length())
				PRINTED.insert(0, longest_text - PRINTED.length(), ' ');
			else if (longest_text < PRINTED.length())
				longest_text = PRINTED.length();

			const unsigned POS_X = console::get_size().first - PRINTED.length() - 1;

			// Wypisanie licznika
			console::gotoxy(POS_X, 1);
			console::print(PRINTED, console::ALL);
			console::gotoxy(TMP);
		}
		// Sprawdzenie żeby nie trzeba było czekać w razie potrzeby
		if (_stop_counter) return;

		// Wstrzymanie wątku na 1s
		std::this_thread::sleep_for(std::chrono::seconds(INTERVAL));
	} while ( ! _stop_counter);
}

/**
 * Czyszczenie okna konsoli od określonej linii
 *
 * @param sudoku aktualna plansza
 * @param y wiersz od którego powinno się zacząć czyszczenie
 */
void clear_after_line(sudoku::board &sudoku, const unsigned y = 0) {
	for (size_t i = console::get_cursor_pos().y; i >= sudoku.additionals_start() + y; --i)
		console::clear_line(i);
	console::gotoxy(0, sudoku.additionals_start() + y);
}

/**
 * Wczytywanie komentarza do komórki
 *
 * @return komentarz
 */
int read_comment() {
	// Pobieranie komentarza i czekanie na potwierdzenie
	int tmp = 0, comment = 0;
	const auto POS = console::get_cursor_pos();
	do {
		console::gotoxy(POS.x, POS.y);
		comment = tmp;
		tmp = console::getche();
	} while (tmp != 13 || comment <= '0' || comment > '9');

	return comment;
}

/**
 * Handlery dla odpowiednich funkcji sudoku
 */
namespace handle {
	/**
	 * Usuwanie wartości komórki
	 *
	 * @param  sudoku plansza z której ma być usunięta wartość
	 * @param  undo_redo lista undo-redo
	 * @return void
	 */
	void del(sudoku::board &sudoku, list &undo_redo) {
		clear_after_line(sudoku);
		sudoku.print(console::string());
		undo_redo.add(sudoku);
	}

	/**
	 * Obsługa 'znaków specjalnych' → strzałek/delete
	 *
	 * @param  sudoku plansza po której się poruszamy
	 * @param  undo_redo lista undo-redo
	 * @param  ch znak wczytany z klawiatury
	 * @return void
	 */
	void special_chars(sudoku::board &sudoku, list &undo_redo, const int ch) {
		clear_after_line(sudoku);
		if (ch == 0x48) // ↑
			sudoku.goto_cell(sudoku::board::direction::TOP);
		else if (ch == 0x50) // ↓
			sudoku.goto_cell(sudoku::board::direction::BOTTOM);
		else if (ch == 0x4b) // ←
			sudoku.goto_cell(sudoku::board::direction::LEFT);
		else if (ch == 0x4d) // →
			sudoku.goto_cell(sudoku::board::direction::RIGHT);
		else if (ch == 83) // Delete
			del(sudoku, undo_redo);
	}

	/**
	 * Wykonywanie operacji na liście undo-redo
	 *
	 * @param  sudoku plansza na której jest cofany/ponawiany ruch
	 * @param  undo_redo lista undo-redo
	 * @param  ch znak wpisany na klawiaturze
	 * @return void
	 */
	void undo_redo(sudoku::board &sudoku, list &undo_redo, const int ch) {
		// Właściwa obsługa undo-redo
		if (tolower(ch) == 'u') {
			// Wyczyszczenie poprzednich komunikatów
			clear_after_line(sudoku);
			sudoku = *undo_redo.undo();
		}
		else if (tolower(ch) == 'r') {
			// Wyczyszczenie poprzednich komunikatów
			clear_after_line(sudoku);
			sudoku = *undo_redo.redo();
		}
	}

	/**
	 * Prosta podpowiedź dla aktualnego pola
	 *
	 * @param  sudoku plansza dla której ma być podpowiedź
	 * @return void
	 */
	void curr_prompt(sudoku::board &sudoku) {
		// Wyczyszczenie poprzednich komunikatów
		clear_after_line(sudoku);

		console::print(_("Wartości nie powodujące konfliktu:"));
		const bool *vals = sudoku.curr_prompt();
		console::string tmp;
		// Tworzenie listy pasujących wartości
		for (size_t i = 0; i < 9; ++i) {
			tmp += vals[i] ? std::to_wstring(i + 1) + _(", ") : console::string();
		}
		// Usunięcie ew. średnika na końcu
		if (tmp.back() == _(' ')) tmp.erase(tmp.length() - 2);
		console::print(tmp);
	}

	/**
	 * Jednoznaczna odpowiedź
	 * Jeśli użytkownik potwierdzi wybór, dodaje do listy undo-redo
	 *
	 * @param  sudoku plansza dla której ma być podpowiedź
	 * @param  undo_redo lista undo-redo
	 * @return void
	 */
	void explicit_prompt(sudoku::board &sudoku, list &undo_redo) {
		// Wyczyszczenie poprzednich komunikatów
		clear_after_line(sudoku);

		auto res = sudoku.explicit_prompt();
		sudoku.print(res.first.x, res.first.y, _("→") + std::to_wstring(res.second));
		// Potwierdzenie + ew. wpisanie wartości
		console::print(_("Czy chcesz wstawić tę liczbę? (T/n)"));
		int confirm = console::getch();
		// Użytkownik potwierdził podpowiedź
		if (confirm == 'T') {
			sudoku.print(res.first.x, res.first.y, std::to_wstring(res.second));
			undo_redo.add(sudoku);
		}
		// Użytkownik nie potwierdził wyboru
		else sudoku.print(res.first.x, res.first.y, console::string());
		// Fix na znaki specjalne
		if (confirm == 0) console::getch();

		clear_after_line(sudoku);
	}

	/**
	 * Wyróżnienie określonej liczby na planszy
	 *
	 * @param  sudoku plansza na której ma być zaznaczona wartość
	 * @return void
	 */
	void denote(sudoku::board &sudoku) {
		// Wyczyszczenie poprzednich komunikatów
		clear_after_line(sudoku);

		console::print(_("Jaką liczbę chcesz oznaczyć?"));
		// Wczytanie PRAWIDŁOWEJ liczby
		int confirm = 0;
		do {
			confirm = console::getch();
			// Fix na znaki specjalne
			if (confirm == 0) console::getch();
		} while (confirm < '0' || confirm > '9');
		console::clear_line(sudoku.additionals_start());
		// Alternatywny styl dla wyróżnienia
		sudoku.toggle_alternative(std::to_wstring(confirm - '0'));
		// Zaznaczenie wszystkich liczb na inny kolor
		sudoku.redraw_all(false);
	}

	/** 
	 * Lista komentarzy dla aktualnego pola
	 *
	 * @param  sudoku plansza na której mają być pobrane komentarze
	 * @return void
	 */
	void _comments_list(sudoku::board &sudoku) {
		const auto res = sudoku.get_comments();
		if (res.second) { // Są jakieś komentarze
			console::string comments;
			for (size_t i = 0; i < res.second; ++i)
				comments += std::to_wstring(res.first[i]) + (i + 1 == res.second ? console::string() : _(", "));
			console::print(_("Komentarze wpisane do aktualne pola: ") + comments);
		}
		else console::print(_("Brak komentarzy dla aktualnego pola"));
	}

	/**
	 * Dodawanie komentarza
	 *
	 * @param  sudoku plansza na której będzie dodany komentarz
	 * @param  undo-redo lista undo-redo do której zostanie dodana operacja
	 * @return void
	 */
	void _add_comment(sudoku::board &sudoku, list &undo_redo) {
		console::print(_("Wpisz komentarz (enter potwierdza):"));
		// Pobieranie komentarza i czekanie na potwierdzenie
		const int comment = read_comment();
		// Wyświetlenie potwierdzenia i zapis
		console::print(_("Wpisany komentarz: ") + std::to_wstring(comment - '0'));
		sudoku.add_comment(static_cast<short>(comment - '0'));
		console::print(_("Wciśnij dowolny klawisz aby kontynuować"));
		console::getch();
		undo_redo.add(sudoku);
	}

	/**
	 * Usuwanie komentarza
	 *
	 * @param  sudoku plansza na której będzie usunięty komentarz
	 * @param  undo-redo lista undo-redo do której zostanie dodana operacja
	 * @return void
	 */
	void _remove_comment(sudoku::board &sudoku, list &undo_redo) {
		console::print(_("Wpisz liczbę którą chcesz usunąć (enter potwierdza):"));
		const int comment = read_comment();
		// Kasowanie komentarza
		sudoku.remove_comment(static_cast<short>(comment - '0'));
		undo_redo.add(sudoku);
	}

	/**
	 * Dodanie/usunięcie komentarza dla aktualnej komórki
	 *
	 * @param  sudoku plansza dla której będzie zmieniony komentarz
	 * @param  undo-redo lista undo-redo do której zostanie dodana operacja
	 * @return void
	 */
	void comment(sudoku::board &sudoku, list &undo_redo) {
		// Wyczyszczenie poprzednich komunikatów
		clear_after_line(sudoku);

		// Lista komentarzy dla tego pola
		_comments_list(sudoku);
		// Lista wyboru
		console::print(_("Co chcesz zrobić?"));
		console::print(_("a) dodać komentarz"));
		console::print(_("b) usunąć komentarz"));
		// Wybrana przez użytkownika opcja
		const int choice = console::getch();
		// Czyszczenie menu wyboru
		clear_after_line(sudoku, 1);

		// Użytkownik chce dodać komentarz
		if (tolower(choice) == 'a')
			_add_comment(sudoku, undo_redo);
		// Użytkownik chce usunąć komentarz
		else if (tolower(choice) == 'b')
			_remove_comment(sudoku, undo_redo);
		// Fix na znaki specjalne
		else if (choice == 0) console::getch();

		// Wyczyszczenie stanu konsoli
		clear_after_line(sudoku);
	}

	/**
	 * Wstawienie/zmiana liczby
	 *
	 * @param  sudoku plansza na której jest zmieniana liczba
	 * @param  undo-redo lista undo-redo do której zostanie dodana operacja
	 * @param  ch znak wpisany na klawiaturze
	 * @return void
	 */
	void digit(sudoku::board &sudoku, list &undo_redo, const int ch) {
		if ( ! sudoku.conflicts(ch - '0')) {
			sudoku.print(std::to_wstring(ch - '0'));
			undo_redo.add(sudoku);
		}
	}

	/**
	 * Wczytywanie wartości komórek z XML'a
	 *
	 * @param  node węzeł XML'a do przetworzenia
	 * @return para z buforem zawierającym komórki i ilością zainicjalizowanych pól
	 */
	pair<sudoku::cell*, size_t> xml_read_cells(XML::Reader &node) {
		// Wczytywanie listy komórek
		sudoku::cell *initial = new sudoku::cell[81];
		std::wstringstream ss;
		ss << node.get_node(_("board")).inner_text();
		console::string tmp;
		size_t count = 0, nr = 0;
		// Przetwarzanie kolejnych pól
		while (ss >> tmp) {
			if (tmp != _("-")) {
				initial[count++] = sudoku::cell(nr % 9, nr / 9, std::stoi(tmp));
			}
			else { // Inicjalizacja pustych komórek
				initial[count++] = sudoku::cell(nr % 9, nr / 9, 0);
			}
			++nr;
		}

		return pair<sudoku::cell*, size_t>(initial, count);
	}

	/**
	 * Wczytywanie komentarzy dla określonego pola
	 *
	 * @param  node węzeł XML'a zawierający stan
	 * @param  initial tablica z komórkami do inicjalizacji
	 * @param  count ilość zaincjalizowanych komórek
	 * @return void
	 */
	sudoku::cell *xml_read_comments(XML::Reader &node, sudoku::cell *initial, const size_t count) {
		auto comments = node.get_nodes(_("comment"));
		for (size_t j = 0; j < comments.second; ++j) {
			const size_t row = std::stoi(comments.first[j].attrib(_("row"))), col = std::stoi(comments.first[j].attrib(_("col")));
			size_t nr = 0;
			std::wstringstream ss;
			ss << comments.first[j].inner_text();
			console::string tmp;
			// Szukanie odpowiedniego pola (można by zrobić binary search...)
			for (size_t k = 0; k < count; ++k) {
				// Wiersz i kolumna się zgadzają
				if (initial[k].x == col - 1 && initial[k].y == row - 1) {
					while (ss >> tmp) initial[k].comments[nr++] = std::stoi(tmp);
				}
			}
		}
		return initial;
	}

	/** 
	 * Obsługa wczytywania XML'a
	 *
	 * @param  POS_X kolumna w której ma się wyświetlać sudoku
	 * @param  POS_Y wiersz w której ma się wyświetlać sudoku
	 * @param  undo_redo lista undo-redo
	 * @return void
	 */
	void xml_read(const console::string &filename, size_t POS_X, const size_t POS_Y, list &undo_redo) {
		XML::Reader file(filename);
		// Pobieranie root node'a
		file = file.get_node(_("sudoku"));
		// Lista stanów
		auto nodes = file.get_nodes(_("state"));
		sort(nodes.first, nodes.first + nodes.second);
		// Przetwarzanie kolejnych stanów
		for (size_t i = 0; i < nodes.second; ++i) {
			// Inicjalizacja stanu
			sudoku::board state(POS_X, POS_Y, false);
			// Wczytywanie wartości komórek
			auto cells = xml_read_cells(nodes.first[i]);
			size_t count = cells.second;
			auto *initial = cells.first;

			// Przetwarzanie komentarzy
			initial = xml_read_comments(nodes.first[i], initial, count);

			// Inicjalizacja i dodanie do listy undo/redo
			state.init_vals(initial, count);
			undo_redo.add(state, std::stoi(nodes.first[i].attrib(_("nr"))));
			delete[] initial;
		}
		// Ustawienie odpowiedniego stanu jako aktualny
		undo_redo.set_as_current( std::stoi(file.attrib(_("active-state"))) );
		undo_redo.get_current()->redraw_all(false);
	}

	/**
	 * Wczytywanie ciągu znaków do stringa
	 *
	 * @return wczytany string
	 */
	console::string read_into_buffer() {
		console::string buffer;
		int ch;
		while ((ch = console::getche()) != '\r') {
			const size_t line = console::get_cursor_pos().y;
			// Backspace = kasowanie ostatniego znaku z bufora
			if (ch == 0) console::getch(); // Fix na znaki specjalne
			else if (ch == 8) {
				buffer.pop_back();
				console::clear_line(line);
				console::gotoxy(0, line);
				console::print(buffer);
			}
			else buffer.push_back(ch);
			console::gotoxy(buffer.length(), line);
		}

		return buffer;
	}

	/**
	 * Wczytywanie pliku
	 * Pyta o nazwę pliku i wywołuje handle::xml_read()
	 *
	 * @param  POS_X kolumna w której ma się wyświetlać sudoku
	 * @param  POS_Y wiersz w której ma się wyświetlać sudoku
	 * @param  undo_redo lista undo-redo
	 * @param  aktualnie używana plansza sudoku
	 * @return void
	 */
	void file_read(const size_t POS_X, const size_t POS_Y, list &undo_redo, sudoku::board &sudoku) {
		console::print(_("Wpisz nazwę pliku: "));
		console::string buffer = read_into_buffer();
		// Czyszczenie komunikatów
		const size_t line = console::get_cursor_pos().y;
		console::clear_line(line - 1);
		console::clear_line(line);
		// Sprawdzenie czy plik istnieje
		std::wifstream file(buffer);
		if ( ! file.good()) return;
		file.close();

		undo_redo.reset();
		xml_read(buffer, POS_X, POS_Y, undo_redo);
		sudoku = *(undo_redo.get_current());
	}

	/**
	 * Zapis do pliku XML
	 *
	 * @param  undo_redo lista undo-redo do serializacji
	 * @return void
	 */
	void file_save(list &undo_redo) {
		console::print(_("Wpisz nazwę pliku: "));
		console::string filename = read_into_buffer();
		// Czyszczenie komunikatów
		const size_t line = console::get_cursor_pos().y;
		console::clear_line(line - 1);
		console::clear_line(line);
		// Sprawdzenie czy plik istnieje
		std::wofstream file(filename);
		if ( ! file.good()) return;
		file << undo_redo.serialize();
		file.close();
	}

	/**
	 * Obsługa UI
	 *
	 * @param  POS_X kolumna w której ma się wyświetlać sudoku
	 * @param  POS_Y wiersz w której ma się wyświetlać sudoku
	 * @param  sudoku aktualna plansza sudoku
	 * @param  undo_redo lista undo-redo
	 * @return void
	 */
	void ui(const size_t POS_X, const size_t POS_Y, sudoku::board &sudoku, list &undo_redo) {
		int ch;
		bool _is_endgame = false;
		do {
			ch = console::getch();
			std::lock_guard<std::mutex> lock(_time_counter_mutex);
			// Znaki specjalne
			if (ch == 0) handle::special_chars(sudoku, undo_redo, console::getch());
			// Podpowiedź z punktu 1a)
			if (tolower(ch) == 'l') handle::curr_prompt(sudoku);
			// Podpowiedź z punktu 1b)
			else if (tolower(ch) == 'p') handle::explicit_prompt(sudoku, undo_redo);
			// Oznaczanie konkretnej liczby
			else if (tolower(ch) == 'w') handle::denote(sudoku);
			// Tryb edycji komentarza
			else if (tolower(ch) == 'k') handle::comment(sudoku, undo_redo);
			// Wpisywanie wartości
			else if (ch > '0' && ch <= '9') handle::digit(sudoku, undo_redo, ch);
			// Backspace
			else if (ch == 8) handle::del(sudoku, undo_redo);
			// Wczytywanie XML'a
			else if (ch == 'o') {
				handle::file_read(POS_X, POS_Y, undo_redo, sudoku);
				clear_after_line(sudoku);
				_is_endgame = false;
			}
			// Zapis do pliku
			else if (ch == 's') handle::file_save(undo_redo);
			// Undo/Redo
			handle::undo_redo(sudoku, undo_redo, ch);
			// Wykrywanie końca gry
			if ( ! _is_endgame && (_is_endgame = sudoku.is_endgame()) ) {
				console::print(_("Brawo! Gra skończona."), console::ALTERNATIVE);
			}
		} while (tolower(ch) != 'q');
	}
}

/**
 * Porównanie 2 stanów z XML'a
 *
 * @param  r1 pierwszy stan do porównania
 * @param  r2 drugi stan do porównania
 * @return true jeśli r1 < r2
 */
bool operator<(XML::Reader& r1, XML::Reader& r2) {
	if (r1.is_empty() || r2.is_empty()) return true;
	return std::stoi(r1.attrib(_("nr"))) < std::stoi( r2.attrib(_("nr")) );
}

int main()
{
	console::title(_("Tomasz Stasiak 155197"));
	list undo_redo;

	const size_t POS_X = 5, POS_Y = 1;
	// Licznik czasu
	std::thread time_count = std::thread(time_counter, &undo_redo);

	console::cls(); // Czyszczenie ekranu jakby coś było wcześniej uruchamiane itp.
	sudoku::board sudoku(POS_X, POS_Y);
	undo_redo.add(sudoku);
	// Rysowanie helpa i licznika ruchów
	undo_redo.draw_help().draw_counter();

	// Można normalnie zastartować wątek z timerem
	_start_counter = true;

	// Funkcja obsługująca UI
	handle::ui(POS_X, POS_Y, sudoku, undo_redo);

	_stop_counter = true;
	OutputDebugString(_("Oczekiwanie na wątki..."));
	// Oczekiwanie na wątek
	time_count.join();

	return 0;
}
