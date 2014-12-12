#include "console.h"
#include "utils.h"
#include <fstream>
#include <sstream>

/**
 * Uproszczona obs�uga XML'a
 * Brakuje sporej ilo�ci funkcji, poza tym przetwarzanie jest cz�ciowo dostosowane do wygl�du XML'a
 */
namespace XML {

	// Co jest traktowane jako bia�e znaki?
	const console::string WHITESPACE = _(" \t\r\n");

	/**
	 * Klasa odpowiedzialna za wczytywanie XML'a
	 */
	class Reader {
	public:
		Reader(const console::string &file_name);
		Reader() = default;
		Reader get_node(const console::string &name);
		pair<Reader*, size_t> get_nodes(const console::string &name);
		const console::string inner_text();
		const console::string outer_text() { return _content; };
		const console::string attrib(const console::string &name);

		inline const bool is_empty() { return !_content.length(); }
	private:
		// Typ przechowuj�cy pojedynczy atrybut
		typedef pair<console::string, console::string> attr_type;

		// Init w�z�a
		Reader(const console::string &content, const bool node_init) { if (node_init) _content = content; }
		Reader& get_attribs();
		// Trimowanie stringa
		const console::string rtrim(const console::string &val);
		const console::string ltrim(const console::string &val);
		const console::string trim(const console::string &val);
		void clear_whitespace_after(const console::string &str);
		void erase_fragment(const size_t from, const size_t to);

		// Nazwa ostatniego wczytywanego w�z�a
		console::string _last_node;
		// Nieprzetworzona tre�� ca�ego pliku XML
		console::string _content;
		// Tablica zawieraj�ca atrybuty aktualnego w�z�a
		attr_type *_attribs = nullptr;
		size_t _attribs_count = 1;
		// Inner text
		console::string _inner_text;
		size_t _cur_pos = 0;
	};
}