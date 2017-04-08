#include <stddef.h>

#include "usb_keyboard.h"
#include "keys.hpp"
#include "test.hpp"

template <class Kbd>
class firmware {
    void run();
};

template <class List, size_t N>
struct index_typelist;

template <template <class...> class List, class T, class... Ts, size_t N>
struct index_typelist<List<T,Ts...>, N> {
    using type = typename index_typelist<List<Ts...>, N-1>::type;
};

template <template <class...> class List, class T, class... Ts>
struct index_typelist<List<T,Ts...>, 0> {
    using type = T;
};

template <class List, size_t N>
struct index_vallist;

template <template <auto...> class List, auto T, auto... Ts, size_t N>
struct index_vallist<List<T,Ts...>, N> {
    static constexpr auto value = index_typelist<List<Ts...>, N-1>::value;
};

template <template <auto...> class List, auto T, auto... Ts>
struct index_vallist<List<T,Ts...>, 0> {
    static constexpr auto value = T;
};

template <class List1, class List2>
struct cat_typelist;

template <template <auto...> class List, auto... Ts, auto... Us>
struct cat_typelist<List<Ts...>, List<Us...>> {
    using type = List<Ts...,Us...>;
};

template <pin Col, class RowPinset, class Row>
struct build_pin_set;

template <pin Col, pin P, pin... Ps, int... Bs>
struct build_pin_set<Col, pin_set<P,Ps...>, row<1,Bs...>> {
    using type = typename cat_typelist<pin_set<P>,
                              typename build_pin_set<Col, pin_set<Ps...>, row<Bs...>>::type>::type;
};

template <pin Col, pin P, pin... Ps, int... Bs>
struct build_pin_set<Col, pin_set<P,Ps...>, row<0,Bs...>> {
    using type = typename build_pin_set<Col, pin_set<Ps...>, row<Bs...>>::type;
};

template <pin Col, class RowPinset>
struct build_pin_set<Col, RowPinset, row<>> {
    using type = pin_set<>;
};


template <class ColumnPins, class RowPins, class Pos>
struct build_all_pin_sets;

template <pin... ColumnPins, class RowPins, class... Rows>
struct build_all_pin_sets<pin_set<ColumnPins...>, RowPins, typelist<Rows...>> {
    using type = typelist<typename build_pin_set<ColumnPins, RowPins, Rows>::type...>;
};

template <class Kbd>
using scan_set_for =
    typename build_all_pin_sets<typename Kbd::columns,
                                typename Kbd::rows,
                                typename Kbd::key_positions>::type;

template <class Layout, pin ColPin, size_t ColN, pin RowPin, size_t RowN>
void test_key (int* pressed) {
    //TODO add condition
    if (true) {
        //TODO fix pressed
        pressed[0] = index_vallist<typename index_typelist<typelist, ColN>::type, RowN>::value;
    }
}

template <class Layout, pin RowPin, size_t RowN, pin... ColPins, size_t ColNs>
void scan_columns(int* pressed, pin_set<ColPins...>, index_sequence<ColNs...>) {
    (test_key<Layout, RowPin, RowN, ColPin, ColNs>(pressed), ...);
}

template <class Layout, pin... RowPins, size_t... RowIdxs, class... RowScans>
void matrix_scan(pin_set<RowPins...>, index_sequence<RowIdxs...>, typelist<ColScans...>) {
    static int pressed[6];

    (scan_columns<Layout, RowPins, RowIdxs>(pressed, ColScans, sequence_for_vallist<ColScans...>>}), ...);
}

template <class Kbd>
void firmware<Kbd>::run() {
    using scan_set = scan_set_for<Kbd>;
    auto pressed = matrix_scan<decltype(Kbd::layouts()), (typename Kbd::rows, scan_set{});

    for (auto i = 0; i < std::min(6, pressed.size()); ++i) {
        keyboard_keys[i] = pressed[i];
    }
    usb_keyboard_send();
}


template <class List> struct sequence_for_vallist_impl;

template <template <auto...> class List, auto... Vals>
struct sequence_for_vallist_impl<List<Vals...>> {
    using type = std::make_index_sequence<sizeof...(Vals)>;
};

template<class List>
using sequence_for_vallist = typename sequence_for_vallist_impl<List>::type;

template <class Kbd, class... Rows>
void validate_layout(type<layout<Rows...>>) {
    static_assert(sizeof...(Rows) == variadic_size<Kbd::rows>(), "A layout has the wrong number of rows");
}


template <class Kbd>
void validate_keyboard() {
    (validate_layout<Kbd>(type<Kbd::layouts>), ...);
}

int main() {
    validate_keyboard<keyboard_to_run>();
    firmware<keyboard_to_run> frm;
    frm.run();
}
