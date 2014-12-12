#pragma once
/**
 * Przydatne funkcje zaimplementowane z u¿yciem programowania uogólnionego
 * (c) Tomasz Stasiak 2014
 */

/**
 * Zmiana rozmiaru bufora jeœli jest to potrzebne
 * Taki std::vector dla ubogich
 *
 * @template buff_type typ danych przechowywany w tablicy
 * @param    buff referencja do tablicy która ma byæ sprawdzona i ew. rozszerzona
 * @param    size aktualny rozmiar tablicy (zostanie nadpisany)
 * @param    count rzeczywista iloœæ elementów w tablicy
 * @return   void
 */
template<class buff_type>
void resize_buffer(buff_type *&buff, size_t &size, const size_t count) {
	if (count == size) {
		// Resize bufora i przepisanie danych
		buff_type *tmp = new buff_type[size *= 2];
		for (size_t i = 0; i < size / 2; ++i)
			tmp[i] = buff[i];

		// Zamiana bufora i czyszczenie po poprzednim
		if (buff != nullptr) delete[] buff;
		buff = tmp;
	}
}

/**
 * Swap 2 zmiennych
 * Wymaga istnienia operatora przypisania dla typu 'type'
 *
 * @template type typ zamienianych zmiennych
 * @param    a pierwsza zmienna
 * @param    b druga zmienna
 * @return   void
 */
template <class type>
void swap(type &a, type &b) {
	type tmp = a;
	a = b;
	b = tmp;
}

/**
 * Zamiana wartoœci zmiennych wskazywanych przez pointery
 * Wymaga istnienia operatora przypisania dla typu 'type'
 *
 * @template type typ zamienianych zmiennych
 * @param    a pointer do pierwszej zmiennej
 * @param    b pointer do drugiej zmiennej
 * @return   void
 */
template <class type>
void iter_swap(type *&a, type *&b) {
	swap(*a, *b);
}

/**
 * Quicksort z w³asnym komparatorem
 *
 * @template type typ zmiennych przechowywanych w tablicy/kontenerze
 * @template compare typ komparatora
 * @param    first wkaŸnik do pierwszego elementu sortowanego przedzia³u
 * @param    last pointer do ostatniego elementu sortowanego przedzia³u
 * @param    cmp komparator
 * @return   void
 */
template<class type, class compare>
void sort(type *first, type *last, compare cmp) {
	if (first != last) {
		type *left = first, *right = last, *pivot = left++;
		while (left != right) {
			if (cmp(*left, *pivot)) {
				++left;
			}
			else {
				while ((left != right) && cmp(*pivot, *right))
					--right;
				iter_swap(left, right);
			}
		}
		--left;
		iter_swap(pivot, left);

		sort(first, left, cmp);
		sort(right, last , cmp);
	}
}

/**
 * Quicksort wykorzystuj¹cy prze³adowany operator porównania
 *
 * @template type typ danych przechowywany w tablicy
 * @param    first wkaŸnik do pierwszego elementu sortowanego przedzia³u
 * @param    last pointer do ostatniego elementu sortowanego przedzia³u
 * @return   void
 */
template<class type>
void sort(type *first, type* last) {
	sort(first, last, operator<);
}