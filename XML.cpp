#include "XML.h"

namespace XML {

	/**
	 * Wczytywanie z pliku
	 *
	 * @param filename nazwa/ścieżka do pliku w którym jest XML
	 */
	Reader::Reader(const console::string &filename) {
		// Otwarcie pliku
		std::wifstream file(filename, std::ios::in | std::ios::binary);
		// Ostringstream dla szybszego wczytania
		// Why? http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
		std::wostringstream content;
		// Kopiowanie bufora
		content << file.rdbuf();
		file.close();

		_content = content.str();

		// Wstępne kasowanie zbędnych białych znaków przed nazwą węzłów
		clear_whitespace_after(_("<"));
		clear_whitespace_after(_("</"));
	}

	/**
	 * Czyszczenie białych znaków po otwarciu/zamknięciu tagu
	 *
	 * @param str tekst po którym ma być czyszczenie
	 * @return void
	 */
	void Reader::clear_whitespace_after(const console::string &str) {
		size_t pos = 0;
		while ((pos = _content.find(str, pos + 1)) != console::string::npos) {
			const size_t white_end = _content.find_first_not_of(WHITESPACE, pos + 1);
			erase_fragment(pos, white_end);
		}
	}

	/**
	 * Wymazywanie fragmentu wczytanego dokumentu
	 *
	 * @param  from pozycja od której ma być wymazanie
	 * @param  to pozycja do której ma być wykasowanie
	 * @return void
	 */
	void Reader::erase_fragment(const size_t from, const size_t to) {
		if (to - from != 1) {
			_content.erase(from + 1, to - from - 1);
		}
	}

	/**
	 * Wczytywanie atrybutów dla aktualnego węzła
	 *
	 * @return *this
	 */
	Reader& Reader::get_attribs() {
		const size_t attribs_start = _content.find_first_of(WHITESPACE);
		const size_t attribs_end = _content.find(_(">"));
		if (attribs_start > attribs_end) return *this;

		if (_attribs == nullptr)
			_attribs = new attr_type[_attribs_count];

		// Wycięcie stringa zawierającego atrybuty
		const console::string attribs_str = trim(_content.substr(attribs_start, attribs_end - attribs_start - 1));
		// Jeśli są tylko białe znaki
		if (attribs_str.length() == 0) return *this;
		// Przetwarzanie atrybutów
		std::wstringstream ss;
		ss << attribs_str;
		// Wartości tymczasowe
		console::string curr_attr, name, val;
		// Ilość wczytanych atrybutów
		size_t count = 0;
		// Przetwarzanie kolejnych atrybutów
		while (ss >> curr_attr) {
			// Wycinanie nazwy/wartości atrybutu
			name = curr_attr.substr(0, curr_attr.find(_("=")));
			val = curr_attr.substr(curr_attr.find(_("=")) + 2);
			// Usuwanie cudzysłowu z wartości jeśli został
			if (val.find(_("\"")) != console::string::npos) val.erase(val.find(_("\"")));
			// Resize bufora jeśli jest to potrzebne
			resize_buffer<attr_type>(_attribs, _attribs_count, count);
			// Dopisanie atrybutu
			_attribs[count++] = attr_type(name, val);
		}
		_attribs_count = count;

		return *this;
	}

	///**
	// * Zmiana rozmiaru bufora jeśli jest to potrzebne
	// * Taki std::vector dla ubogich
	// *
	// * @template buff_type typ danych przechowywany w tablicy
	// * @param  buff referencja do tablicy która ma być sprawdzona i ew. rozszerzona
	// * @param  size aktualny rozmiar tablicy (zostanie nadpisany)
	// * @param  count rzeczywista ilość elementów w tablicy
	// * @return void
	// */
	//template<class buff_type>
	//void Reader::resize_buffer(buff_type *&buff, size_t &size, const size_t count) {
	//	if (count == size) {
	//		// Resize bufora i przepisanie danych
	//		buff_type *tmp = new buff_type[size *= 2];
	//		for (size_t i = 0; i < size / 2; ++i)
	//			tmp[i] = buff[i];

	//		// Zamiana bufora i czyszczenie po poprzednim
	//		if (buff != nullptr) delete[] buff;
	//		buff = tmp;
	//	}
	//}
	//// Fix dla templatów
	//template void Reader::resize_buffer <Reader::attr_type>(Reader::attr_type*&, size_t&, const size_t);
	//template void Reader::resize_buffer <Reader>(Reader*&, size_t&, const size_t);

	/**
	 * Pobieranie określonego atrybutu dla określonego węzła
	 *
	 * @param  name nazwa atrybutu
	 * @return wartość, jeśli atrybut istnieje; pusty string w przeciwnym wypadku
	 */
	const console::string Reader::attrib(const console::string &name) {
		if (_attribs == nullptr) return console::string();

		for (size_t i = 0; i < _attribs_count; ++i) {
			if (_attribs[i].first == name) return _attribs[i].second;
		}
		return console::string();
	}

	/**
	 * Pobieranie kolejnego podwęzła XML
	 *
	 * @param  name nazwa węzła do wyszukania
	 * @return *this
	 */
	Reader Reader::get_node(const console::string &name) {
		// Jeśli teraz pobieramy inny węzeł niż ostatnio, zerowanie pozycji
		if (name != _last_node) _cur_pos = 0;
		// Zmiana nazwy ostatniego węzła
		_last_node = name;

		// Początek węzła
		size_t node_start;
		wchar_t next_char;
		// Po nazwie węzła musi być biały znak/ostry nawias
		do {
			node_start = _content.find(_("<") + name, _cur_pos);
			// Sprawdzenie, czy nie doszliśmy do końca pliku bez węzła
			if (node_start == console::string::npos)
				return XML::Reader();
			// Kolejny znak po nazwie
			next_char = _content[node_start + name.length() + 1];
			// Zmiana pozycji od której będziemy szukać
			_cur_pos = node_start + name.length();
		} while ( ! iswspace(next_char) && next_char != _('>'));

		// Koniec węzła
		const size_t node_end = _content.find(_("</") + name + _(">"), node_start);
		// Nie ma zamknięcia
		if (node_end == console::string::npos)
			return XML::Reader();

		// Koniec taga zamykającego węzeł
		const size_t node_end_tag = node_end + (_("</") + name + _(">")).length();
		// Przesunięcie wskaźnika pozycji
		_cur_pos = node_end_tag;

		// Tworzenie węzła
		XML::Reader node(_content.substr(node_start, node_end_tag), true);
		const size_t node_inner_start = _content.find(_(">"), node_start) + 1;
		const size_t node_inner_len = node_end - _content.find(_(">"), node_start) - 1;
		node._inner_text = trim(_content.substr(node_inner_start, node_inner_len));

		return node.get_attribs();
	}

	/**
	 * Pobieranie wszystkich węzłów o danej nazwie
	 *
	 * @param  name nazwa węzła
	 * @return pair<węzły, ilość>
	 */
	pair<Reader*, size_t> Reader::get_nodes(const console::string &name) {
		size_t count = 0, size = 1;
		// Bufor z węzłami
		Reader* nodes = new Reader[size];
		Reader node; // Aktualny węzeł
		// Dopóki węzły nie są puste
		while ( ! (node = get_node(name)).is_empty()) {
			// Jeśli kończy się miejsce w buforze → resize + przepisanie
			resize_buffer<Reader>(nodes, size, count);
			nodes[count++] = node;
		}

		return pair<Reader*, size_t>(nodes, count);
	}

	/**
	 * Getter dla tekstu wewnątrz węzła
	 * 
	 * @return inner_text dla aktualnego węzła
	 */
	const console::string Reader::inner_text() {
		return _inner_text;
	}

	/**
	 * Czyszczenie białych znaków na końcu stringa
	 *
	 * @param  val string do oczyszczenia
	 * @return wyczyszczony string
	 */
	const console::string Reader::rtrim(const console::string &val) {
		size_t endpos = val.find_last_not_of(WHITESPACE);
		if (console::string::npos != endpos)
			return val.substr(0, endpos + 1);
		return val;
	}

	/**
	 * Czyszczenie białych znaków na początku stringa
	 *
	 * @param  val string do oczyszczenia
	 * @return wyczyszczony string
	 */
	const console::string Reader::ltrim(const console::string &val) {
		size_t startpos = val.find_first_not_of(WHITESPACE);
		if (console::string::npos != startpos)
			return val.substr(startpos);
		return val;
	}

	/**
	 * Czyszczenie białych znaków na obu końcach stringa
	 *
	 * @param  val string do oczyszczenia
	 * @return wyczyszczony string
	 */
	const console::string Reader::trim(const console::string &val) {
		return ltrim(rtrim(val));
	}
}