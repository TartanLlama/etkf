#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>

#include "print.h"
#include <util/delay.h>


#include "keys.hpp"
#include "test.hpp"
#include "index_sequence.hpp"

#include "usb_keyboard_debug.h"

template<class T>
const T& min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

template <class Kbd>
class firmware {
public:
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
    static constexpr auto value = index_vallist<List<Ts...>, N-1>::value;
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

template <class List> struct sequence_for_vallist_impl;

template <template <auto...> class List, auto... Vals>
struct sequence_for_vallist_impl<List<Vals...>> {
    using type = make_index_sequence<sizeof...(Vals)>;
};

template<class List>
using sequence_for_vallist = typename sequence_for_vallist_impl<List>::type;

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


template <class RowPins, class ColumnPins, class Pos>
struct build_all_pin_sets;

template <pin... RowPins, class ColumnPins, class... Rows>
struct build_all_pin_sets<pin_set<RowPins...>, ColumnPins, typelist<Rows...>> {
    using type = typelist<typename build_pin_set<RowPins, ColumnPins, Rows>::type...>;
};

template <class Kbd>
using scan_set_for =
    typename build_all_pin_sets<typename Kbd::rows,
                                typename Kbd::columns,
                                typename Kbd::key_positions>::type;

struct pin_info {
    constexpr pin_info(volatile uint8_t* p_pin, volatile uint8_t* p_ddr, volatile uint8_t* p_port, int p_n)
        : pin{p_pin}, ddr{p_ddr}, port{p_port}, n{p_n}
    {}

    volatile uint8_t* pin;
    volatile uint8_t* ddr;
    volatile uint8_t* port;
    int n;
};

template <pin P>
constexpr pin_info get_pin_info() {
    if constexpr (P == b0) { return pin_info{ &PINB, &DDRB, &PORTB, 0 }; }
    else if constexpr (P == b1) { return pin_info{ &PINB, &DDRB, &PORTB, 1 }; }
    else if constexpr (P == b2) { return pin_info{ &PINB, &DDRB, &PORTB, 2 }; }
    else if constexpr (P == b3) { return pin_info{ &PINB, &DDRB, &PORTB, 3 }; }
    else if constexpr (P == b4) { return pin_info{ &PINB, &DDRB, &PORTB, 4 }; }
    else if constexpr (P == b5) { return pin_info{ &PINB, &DDRB, &PORTB, 5 }; }
    else if constexpr (P == b6) { return pin_info{ &PINB, &DDRB, &PORTB, 6 }; }
    else if constexpr (P == b7) { return pin_info{ &PINB, &DDRB, &PORTB, 7 }; }
    else if constexpr (P == c6) { return pin_info{ &PINC, &DDRC, &PORTC, 6 }; }
    else if constexpr (P == c7) { return pin_info{ &PINC, &DDRC, &PORTC, 7 }; }
    else if constexpr (P == d0) { return pin_info{ &PIND, &DDRD, &PORTD, 0 }; }
    else if constexpr (P == d1) { return pin_info{ &PIND, &DDRD, &PORTD, 1 }; }
    else if constexpr (P == d2) { return pin_info{ &PIND, &DDRD, &PORTD, 2 }; }
    else if constexpr (P == d3) { return pin_info{ &PIND, &DDRD, &PORTD, 3 }; }
    else if constexpr (P == d4) { return pin_info{ &PIND, &DDRD, &PORTD, 4 }; }
    else if constexpr (P == d5) { return pin_info{ &PIND, &DDRD, &PORTD, 5 }; }
    else if constexpr (P == d6) { return pin_info{ &PIND, &DDRD, &PORTD, 6 }; }
    else if constexpr (P == d7) { return pin_info{ &PIND, &DDRD, &PORTD, 7 }; }
    else if constexpr (P == e2) { return pin_info{ &PINE, &DDRE, &PORTE, 2 }; }
    else if constexpr (P == e6) { return pin_info{ &PINE, &DDRE, &PORTE, 6 }; }
    else if constexpr (P == f0) { return pin_info{ &PINF, &DDRF, &PORTF, 0 }; }
    else if constexpr (P == f1) { return pin_info{ &PINF, &DDRF, &PORTF, 1 }; }
    else if constexpr (P == f4) { return pin_info{ &PINF, &DDRF, &PORTF, 4 }; }
    else if constexpr (P == f5) { return pin_info{ &PINF, &DDRF, &PORTF, 5 }; }
    else if constexpr (P == f6) { return pin_info{ &PINF, &DDRF, &PORTF, 6 }; }
    else if constexpr (P == f7) { return pin_info{ &PINF, &DDRF, &PORTF, 7 }; }

    //TODO assert or something
    return {&PINB, &DDRB, &PORTB, 0};
}


char itoc (int i) {
    switch (i) {
    case 0: return '0';
    case 1:         return '1';
    case 2:         return '2';
    case 3:         return '3';
    case 4:         return '4';
    case 5:         return '5';
    case 6:         return '6';
    case 7:         return '7';
    case 8:         return '8';
    case 9:         return '9';

    }
    return 'x';
}


template <class Layout, pin RowPin, size_t RowN, pin ColPin, size_t ColN>
void test_key (int* pressed) {
    auto info = get_pin_info<ColPin>();

    if (!((*info.pin) & (1 << info.n))) {
        //TODO fix pressed
        using row = typename index_typelist<Layout, RowN>::type;
        pressed[0] = index_vallist<row, ColN>::value;
    }
}

template <pin P> void power_pin() {
    auto info = get_pin_info<P>();

    *info.ddr  |=  (1 << info.n);
    *info.port &= ~(1 << info.n);
}

template <pin P> void unpower_pin() {
    auto info = get_pin_info<P>();

    *info.ddr  &= ~(1 << info.n);
    *info.port &= ~(1 << info.n);
}

template <pin P> void pin_set_for_reading() {
    auto info = get_pin_info<P>();

    *info.ddr  &= ~(1 << info.n);
    *info.port |=  (1 << info.n);
}

template <pin... Ps> void setup_input_pins(pin_set<Ps...>) {
    (pin_set_for_reading<Ps>(), ...);
}

template <class Layout, pin RowPin, size_t RowN, pin... ColPins, size_t... ColNs>
void scan_columns(int* pressed, pin_set<ColPins...>, index_sequence<ColNs...>) {
    power_pin<RowPin>();
    (test_key<Layout, RowPin, RowN, ColPins, ColNs>(pressed), ...);
    unpower_pin<RowPin>();
}

template <class Layout, pin... RowPins, size_t... RowIdxs, class... ColScans>
int* matrix_scan(pin_set<RowPins...>, index_sequence<RowIdxs...>, typelist<ColScans...>) {
    static int pressed[6];

    (scan_columns<Layout, RowPins, RowIdxs>(pressed, ColScans{}, sequence_for_vallist<ColScans>{}), ...);

    return pressed;
}

template <class Kbd>
void firmware<Kbd>::run() {
    while (true) {
        using scan_set = scan_set_for<Kbd>;
        using rows = typename Kbd::rows;
        auto pressed = matrix_scan<decltype(Kbd::layouts())> (rows{}, sequence_for_vallist<rows>{}, scan_set{});

        auto n_pressed = 1; // TODO fix
        for (auto i = 0; i < min(6, n_pressed); ++i) {
            keyboard_keys[i] = pressed[i];
        }

        usb_keyboard_send();

        for (auto i = 0; i < 6; ++i) {
            pressed[i] = 0;
        }
    }
}


/*template <class Kbd, class... Rows>
void validate_layout(type<layout<Rows...>>) {
    static_assert(sizeof...(Rows) == variadic_size<Kbd::rows>(), "A layout has the wrong number of rows");
    }*/


template <class Kbd>
void validate_keyboard() {
//    (validate_layout<Kbd>(type<Kbd::layouts>), ...);
}

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))


int main() {
    CPU_PRESCALE(0);
    usb_init();
    while (!usb_configured()) /* wait */ ;
    _delay_ms(10000);

    //   validate_keyboard<keyboard_to_run>();
    setup_input_pins(keyboard_to_run::columns{});
    firmware<keyboard_to_run> frm;

    frm.run();
}
