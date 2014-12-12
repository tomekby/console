#include "sudoku.h"

namespace sudoku {

	/**
	 * Rysowanie planszy
	 *
	 * @return void
	 */
	void board::draw_board() {
		// Wypisywanie planszy
		for (size_t i = 0; i < 19; ++i) {
			// Przesunięcie kursora na odpowiednie miejsce poniżej
			console::gotoxy(START_X, START_Y + i);
			console::print(empty_board[i]);
		}
	}

	/**
	 * Przerysowanie planszy
	 *
	 * @param  clear_screen czy ekran ma być wyczyszczony
	 * @return *this
	 */
	board& board::redraw_all(const bool clear_screen) {
		if (clear_screen) console::cls();

		draw_board(); // Wyświetlenie planszy
		sudoku::cell cells[81];
		// Zmiana z tablicy dwu- na jednowymiarową
		for (size_t i = 0; i < 9; ++i) {
			for (size_t j = 0; j < 9; ++j) {
				cells[9 * i + j] = values[i][j];
			}
		}
		// Inicjalizacja prawidłowymi komórkami
		init_vals(cells, 81);

		return *this;
	}

	/**
	 * Operator kopiowania
	 * 
	 * @param  old poprzedni obiekt
	 * @return *this
	 */
	board& board::operator=(const board &old) {
		START_X = old.START_X;
		START_Y = old.START_Y;
		cur_pos = old.cur_pos;
		// Przepisanie komórek
		for (size_t i = 0; i < 9; ++i) {
			for (size_t j = 0; j < 9; ++j) {
				values[i][j] = old.values[i][j];
			}
		}

		return *this;
	}

	/**
	 * Inicjalizacja planszy określonymi wartościami
	 * Inicjalizacja NIE zmienia licznika ruchów
	 *
	 * @param *cells tablica zaiwerająca pola planszy do wypełnienia
	 * @param  size  wielkość tablicy z polami
	 * @return void
	 */
	void board::init_vals(const cell *cells, const unsigned short size) {
		for (unsigned short i = 0; i < size; ++i) {
			print(cells[i].x, cells[i].y, cells[i].val, true);
			// Przepisanie komentarzy
			for (unsigned short j = 0; j < 9; ++j) {
				values[cells[i].x][cells[i].y].comments[j] = cells[i].comments[j];
			}
		}
		if (draw_at_startup) {
			goto_cell(direction::NONE);
		}
		draw_at_startup = true;
	}

	/**
	 * Przesuwanie kursora w określonym kierunku z zaznaczeniem aktualnej komórki
	 * Jeśli parametr == sudoku::board::direction::NONE to zostanie zaznaczona aktualna komórka
	 * Funkcja zmienia wewnętrzny wskaźnik pozycji i zaznacza aktualną komórkę; pozycja kursora w konsoli nie zmienia się
	 *
	 * @param  d kierunek przesunięcia kursora
	 * @return void
	 */
	void board::goto_cell(const sudoku::board::direction d) {
		// Sprawdzenie, czy ruch jest możliwy do wykonania
		if ((d == direction::LEFT && cur_pos.x == 0) || (d == direction::RIGHT && cur_pos.x == 8) || (d == direction::TOP && cur_pos.y == 0) || (d == direction::BOTTOM && cur_pos.y == 8)) {
			return;
		}
		// Pozycja kursora w konsoli przed ruchem
		const console::cursor TMP = console::get_cursor_pos();

		short move_x = cur_pos.x, move_y = cur_pos.y;
		// Usunięcie zaznaczenia z ostatniej komórki
		print(move_x, move_y, get_value(move_x, move_y), true);
		// Zmiana pozycji na planszy
		if (d == direction::NONE);
		else if (d == direction::LEFT || d == direction::RIGHT) move_x += 1 * (d == direction::LEFT ? -1 : 1);
		else move_y += 1 * (d == direction::TOP ? -1 : 1);
		// Przesunięcie kursora na odpowiednią pozycję
		console::gotoxy(START_X + 2 + 5 * move_x, START_Y + 1 + 2 * move_y);
		const console::string val = get_value(move_x, move_y);
		if (val.length() == 1 || val.length() == 0) {
			console::print(val + _("_"), use_alternative(val) ? console::ALTERNATIVE : console::ALL);
		}
		cur_pos.x = move_x;
		cur_pos.y = move_y;

		// Powrót do poprzedniej pozycji
		console::gotoxy(TMP);
	}

	/**
	 * Wpisanie wartości do komórki o koordynatach (x,y)
	 * (x,y) to koordynaty w planszy, nie w konsoli
	 *
	 * @param  x    kolumna w której ma być wstawiona wartość
	 * @param  y    wiersz w którym ma być wstawiona wartość
	 * @param  val  wartość do wstawienia
	 * @param  init czy to jest inicjalizacja planszy (nie zmienia licznika)
	 * @return void
	 */
	void board::print(const unsigned short x, const unsigned short y, const console::string &val, const bool init) {
		// Nie można wyjść poza planszę
		if (x >= 9 || y >= 9) return;

		assert(val.length() <= 2);

		values[x][y] = val;
		if (draw_at_startup || ! init) {
			// Pozycja kursora przed wpisaniem wartości do komórki
			const console::cursor TMP = console::get_cursor_pos();
			// Wpisanie wartości do komórki
			console::gotoxy(START_X + 2 + 5 * x, START_Y + 1 + 2 * y);
			console::string new_val = val;
			console::print(new_val.append(2 - val.length(), ' '), use_alternative(val) ? console::ALTERNATIVE : console::ALL);
			// Zaznaczenie aktualnej komórki
			if (!init) goto_cell(direction::NONE);

			// Powrót do poprzedniej pozycji
			console::gotoxy(TMP);
		}
	}
	
	/**
	 * Wpisanie wartości do komórki o koordynatach (x,y)
	 * (x,y) to koordynaty w planszy, nie w konsoli
	 *
	 * @param  x    kolumna w której ma być wstawiona wartość
	 * @param  y    wiersz w którym ma być wstawiona wartość
	 * @param  val  wartość do wstawienia
	 * @return void
	 */
	void board::print(const unsigned short x, const unsigned short y, const console::string &val) {
		print(x, y, val, false);
	}

	/**
	 * Wpisanie wartości do aktualnej komórki
	 *
	 * @param  val  wartość do wstawienia
	 * @return void
	 */
	void board::print(const console::string &val) {
		print(cur_pos.x, cur_pos.y, val);
	}

	/**
	 * Pobieranie komórki o określonych koordynatach
	 *
	 * @param  x kolumna w której jest komórka
	 * @param  y wiersz w którym jest komórka
	 * @return obiekt zawierający komórkę
	 */
	const cell board::get_cell(const unsigned short x, const unsigned short y) {
		assert(x < 9);
		assert(y < 9);

		return values[x][y];
	}

	/**
	 * Pobieranie wartości komórki o określonych koordynatach
	 *
	 * @param  x kolumna w której jest komórka
	 * @param  y wiersz w którym jest komórka
	 * @return wartość komórki jako tekst
	 */
	const console::string board::get_value(const unsigned short x, const unsigned short y) {
		return get_cell(x, y).val;
	}

	/**
	 * Pobieranie wartości komórki o określonych koordynatach
	 *
	 * @param  x kolumna w której jest komórka
	 * @param  y wiersz w którym jest komórka
	 * @return wartość komórki jako liczba lub -1 jeśli w komórce jest wpisana inna wartość
	 */
	const short board::get_num_value(const unsigned short x, const unsigned short y) {
		try {
			return static_cast<unsigned short>( std::stoi(get_value(x, y)) );
		}
		catch (const std::invalid_argument &e) {
			(void)e;
			return -1;
		}
	}

	/**
	 * Sprawdzanie, czy w wierszu będą jakieś konflikty
	 *
	 * @param  x kolumna w której jest sprawdzana komórka
	 * @param  y wiersz w którm jest komórka
	 * @param  val sprawdzana wartość
	 * @return false jeśli nie ma konfliktów
	 */
	const bool board::conflicts_row(const unsigned short x, const unsigned short y, const short val) {
		for (size_t i = 0; i < 9; ++i) {
			if (i != x && get_num_value(i, y) == val) 
				return true;
		}
		return false;
	}

	/**
	 * Sprawdzenie, czy w kolumnie występują konflikty
	 *
	 * @param  x sprawdzana kolumna
	 * @param  y sprawdzany wiersz
	 * @param  val sprawdzana wartość
	 * @return true jeśli występuje konflikt
	 */
	const bool board::conflicts_col(const unsigned short x, const unsigned short y, const short val) {
		for (size_t i = 0; i < 9; ++i) {
			if (i != y && get_num_value(x, i) == val)
				return true;
		}
		return false;
	}

	/**
	 * Sprawdzanie, czy w kwadracie występują konflikty
	 *
	 * @param  x sprawdzana kolumna
	 * @param  y sprawdzany wiersz
	 * @param  val sprawdzana wartość
	 * @return true jeśli występuje konflikt
	 */
	const bool board::conflicts_square(const unsigned short x, const unsigned short y, const short val) {
		// Ograniczenia dla kwadratu
		const unsigned short START_X = x / 3 * 3, END_X = START_X + 3;
		const unsigned short START_Y = y / 3 * 3, END_Y = START_Y + 3;
		// Szukanie takiej samej liczby w kwadracie
		for (size_t i = START_X; i < END_X; ++i) {
			for (size_t j = START_Y; j < END_Y; ++j) {
				if (i != x && j != y && get_num_value(i, j) == val)
					return true;
			}
		}
		return false;
	}

	/**
	 * Sprawdzenie potencjalnych konfliktów
	 *
	 * @param  x kolumna w której jest komórka
	 * @param  y wiersz w którym jest komórka
	 * @param  v sprawdzana wartość
	 * @return T → konflikt, F → brak konfliktu
	 */
	const bool board::conflicts(const unsigned short x, const unsigned short y, const unsigned short v) {
		// Sprawdzenie, czy gdzieś są konflikty
		return conflicts_col(x, y, v) || conflicts_row(x, y, v) || conflicts_square(x, y, v);
	}

	/**
	 * Sprawdzenie potencjalnych konfliktów
	 * Alias dla wersji trójargumentowej - sprawdza dla aktualnego pola
	 *
	 * @param  v sprawdzana wartość
	 * @return T → konflikt, F → brak konfliktu
	 */
	const bool board::conflicts(const unsigned short v) {
		return conflicts(cur_pos.x, cur_pos.y, v);
	}

	/**
	 * Proste podpowiedzi dla aktualnej komórki
	 *
	 * @return tablica z liczbami (T → brak konfliktu, F → konflikt)
	 */
	const bool* board::curr_prompt() {
		bool *non_conflicting = new bool[9];
		for (unsigned short i = 0; i < 9; ++i) non_conflicting[0] = 0;
		// Sprawdzenie jakie liczby nie powodują konfliktu
		for (unsigned short i = 0; i < 9; ++i) {
			non_conflicting[i] = ! conflicts(i + 1);
		}

		return non_conflicting;
	}

	/**
	 * Sprawdzanie, czy liczba może być wstawiona do jednoznacznej podpowiedzi
	 *
	 * @param  x sprawdzana kolumna
	 * @param  y sprawdzany wiersz
	 * @param  k sprawdzana wartość
	 * @param  non_conflicting ostatnia liczba nie powodująca konfliktu
	 * @param  c_count ilość liczb powodujących konflikt w tym polu
	 * @return true jeśli to jest kolejna bezkonfliktowa liczba, false w przeciwnym wypadku
	 */
	const bool board::check_if_conflicting(const unsigned short x, const unsigned short y, const unsigned short k, unsigned short &non_conflicting, unsigned short &c_count) {
		// Nie ma konfliktów dla tej liczby
		if ( ! conflicts(x, y, k)) {
			// Już była jakaś bezkonfliktowa liczba
			if (non_conflicting != 10) return true;
			non_conflicting = k;
		}
		// Zwiększenie ilości konfliktowych liczb
		else ++c_count;

		return false;
	}

	/**
	 * Sprawdzanie, czy jest jakaś jednoznaczna liczba którą można wpisać
	 *
	 * @param  x kolumna w której jest komórka
	 * @param  y wiersz w którym jest komórka
	 * @return liczba która nie powoduje konfliktu (jeśli jest tylko 1) lub 10
	 */
	const unsigned short board::get_unconflicting(const unsigned short x, const unsigned short y) {
		// Ilość możliwych konfliktów dla tej komórki
		unsigned short c_count = 0, non_conflicting = 10;
		// Sprawdzanie kolejnych możliwości
		for (unsigned short k = 1; k <= 9; ++k) {
			if (check_if_conflicting(x, y, k, non_conflicting, c_count))
				return 10;
		}

		// Tylko jedna liczba nie wywołuje konfliktu
		if (c_count == 8) return non_conflicting;
		// Więcej niż jedna liczba nie wywołuje konfliktu lub wszystkie to robią
		return 10;
	}

	const unsigned short board::check_unconflicting_for_empty(const unsigned short x, const unsigned short y) {
		if (get_num_value(x, y) == -1) { // Nie ma nic wpisane
			auto res = get_unconflicting(x, y);
			if (res != 10) // Wynik prawidłowy
				return res;
		}
		return 10;
	}

	/**
	 * Szukanie pola dla którego podpowiedź jest jednoznaczna
	 * @todo: optymalizacja (?) / refactoring (?)
	 *
	 * @return koordynaty pola dla którego pasuje tylko jedna liczba oraz pasująca liczba ([10,10,10] jeśli nie ma takiego pola)
	 */
	const pair<console::cursor, unsigned short> board::explicit_prompt() {
		for (size_t i = 0; i < 9; ++i) {
			for (size_t j = 0; j < 9; ++j) {
				auto res = check_unconflicting_for_empty(i, j);
				// Jeśli jest tylko jedna liczba do wstawienia
				if (res) return pair<console::cursor, unsigned short>(console::cursor(i, j), res);
			}
		}

		return pair<console::cursor, unsigned short>(console::cursor(10, 10), 10);
	}

	/**
	 * Pobieranie listy komentarzy dla określonego pola
	 *
	 * @param  x kolumna w której jest komórka
	 * @param  y wiersz w którym jest komórka
	 * @return tablica komentarzy i ich ilość
	 */
	const pair<unsigned short*, unsigned short> board::get_comments(const unsigned short x, const unsigned short y) {
		unsigned short *comments = new unsigned short[9], j = 0;
		for (size_t i = 0; i < 9; ++i) comments[i] = 0;
		// Pobieranie komentarzy
		for (size_t i = 0; i < 9; ++i) {
			const short TMP = get_cell(x, y).comments[i];
			if (TMP != -1) comments[j++] = TMP;
		}

		return pair<unsigned short*, unsigned short>(comments, j);
	}

	/**
	 * Pobieranie komentarzy dla aktualnego pola
	 *
	 * @return tablica komentarzy i ilość
	 */
	const pair<unsigned short*, unsigned short> board::get_comments() {
		return get_comments(cur_pos.x, cur_pos.y);
	}

	/**
	 * Dodawanie komentarza do określonej komórki sudoku
	 *
	 * @param  x kolumna w której komórka
	 * @param  y wiersz w którym jest komórka
	 * @param  v komentarz do dodania
	 * @return void
	 */
	void board::add_comment(const unsigned short x, const unsigned short y, const short v) {
		const auto comments = get_comments(x, y);
		// Jeśli wszystkie komentarze są dodane, pomijamy
		if (comments.second == 9) return;
		for (size_t i = 0; i < comments.second; ++i) {
			// Jeśli już jest taki komentarz, nie dodajemy
			if (comments.first[i] == v) return;
		}
		// Nie było takiego komentarza to można go dodać
		values[x][y].comments[comments.second] = v;
	}

	/**
	 * Dodawanie komentarza do aktualnej komórki
	 *
	 * @param  v komentarz do dodania
	 * @return void
	 */
	void board::add_comment(const short v) {
		add_comment(cur_pos.x, cur_pos.y, v);
	}

	/**
	 * Usuwanie komentarza z określonej komórki
	 *
	 * @param  x kolumna w której komórka
	 * @param  y wiersz w którym jest komórka
	 * @param  v komentarz do usunięcia
	 * @return void
	 */
	void board::remove_comment(const unsigned short x, const unsigned short y, const short v) {
		for (size_t i = 0; i < 9; ++i) {
			// Jeśli jest taki komentarz, usuwamy
			if (values[x][y].comments[i] == v) {
				values[x][y].comments[i] = -1;
			}
		}
	}

	/**
 	 * Usuwanie komentarza z aktualnej komórki
	 *
	 * @param  v komentarz do usunięcia
	 * @return void
	 */
	void board::remove_comment(const short v) {
		remove_comment(cur_pos.x, cur_pos.y, v);
	}

	/**
	 * Zmiana wartości komórki
	 *
	 * @param  value nowa wartość komórki
	 * @return *this
	 */
	inline cell cell::operator=(const console::string &value) {
		val = value;
		return *this;
	}
	
	/**
	 * Operator przypisania
	 *
	 * @param  old komórka która ma być przypisana do tego obiektu
	 * @return *this
	 */
	cell cell::operator=(const cell old) {
		x = old.x;
		y = old.y;
		for (size_t i = 0; i < 9; ++i)
			comments[i] = old.comments[i];
		val = old.val;

		return *this;
	}

	/**
	 * Operator przypisania
	 *
	 * @param  *old pointer na komórkę która ma być przypisana do tego obiektu
	 * @return *this
	 */
	cell cell::operator=(const cell *old) {
		return (*this) = (*old);
	}
} // End namespace sudoku