#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>

#include "print.h"
#include <util/delay.h>

#include "keys.hpp"
#include "test.hpp"
#include "array.hpp"
#include "index_sequence.hpp"
#include "list_utils.hpp"
#include "static_vector.hpp"

#include "usb_keyboard_debug.h"

// Simple std::min implementation
template<class T>
const T& min(const T& a, const T& b)
{
    return (b < a) ? b : a;
}

// Maps column pins onto key positions and removes blank spaces from the row
//
// Example:
//     Row = b4
//     ColPinset = pin_set<f6, f7, c7, c6>
//     KeyPositions = row<1,1,0,1>
//     type = row<f6, f7,     c6>
//          c7 missed out ^^^
template <pin Row, class ColPinset, class KeyPositions>
struct build_pin_set;

// There is a key at this position, so record a pin to test for it, then recurse
template <pin Row, pin P, pin... Ps, int... Bs>
struct build_pin_set<Row, pin_set<P,Ps...>, row<1,Bs...>> {
    using type = typename cat_typelist<pin_set<P>,
                              typename build_pin_set<Row, pin_set<Ps...>, row<Bs...>>::type>::type;
};

// There is no key at this position, so don't record a pin to test for it, then recurse
template <pin Row, pin P, pin... Ps, int... Bs>
struct build_pin_set<Row, pin_set<P,Ps...>, row<0,Bs...>> {
    using type = typename build_pin_set<Row, pin_set<Ps...>, row<Bs...>>::type;
};

// Base case
template <pin Row, class ColPinset>
struct build_pin_set<Row, ColPinset, row<>> {
    using type = pin_set<>;
};

// Carries out build_pin_set on all rows in a key_positions set
//
// Example:
//     RowPins = pin_set<b4, b6>
//     ColumnPins = pin_set<f6, f7, c7, c6>
//     Pos = typelist<row<1,1,0,1>, row<0,1,1,0>>
//     type = typelist<row<f6,f7,c6>, row<f7,c7>>
template <class RowPins, class ColumnPins, class Pos>
struct build_all_pin_sets;

template <pin... RowPins, class ColumnPins, class... Rows>
struct build_all_pin_sets<pin_set<RowPins...>, ColumnPins, typelist<Rows...>> {
    using type = typelist<typename build_pin_set<RowPins, ColumnPins, Rows>::type...>;
};


// Nice helper for build_all_pin_sets
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

    __builtin_unreachable();
}


template <pin RowPin, size_t RowN, pin ColPin, size_t ColN, class... Layouts>
void test_key (static_vector<array<keys::key,sizeof...(Layouts)>,16>& pressed,
               uint8_t& mod_keys, typelist<Layouts...>) {
    auto info = get_pin_info<ColPin>();

    if (!((*info.pin) & (1 << info.n))) {
        array<keys::key,sizeof...(Layouts)> possible_keys {
            index_vallist<typename index_typelist<Layouts, RowN>::type, ColN>::value...
        };
        pressed.push_back(possible_keys);
    }
}

// Power a pin so that keys on that line can be tested
template <pin P> void power_pin() {
    auto info = get_pin_info<P>();

    //DDR <- 1: sets pin to output mode
    *info.ddr  |=  (1 << info.n);

    //PORT <- 0: sets pin to low output
    *info.port &= ~(1 << info.n);
}

// Stop powering a pin
template <pin P> void unpower_pin() {
    auto info = get_pin_info<P>();

    //DDR <- 0: sets pin to input mode
    *info.ddr  &= ~(1 << info.n);

    //PORT <- 0: sets pin to normal mode
    *info.port &= ~(1 << info.n);
}

// Set up a pin for reading at a later time
template <pin P> void pin_set_for_reading() {
    auto info = get_pin_info<P>();

    //DDR <- 0: sets pin to input mode
    *info.ddr  &= ~(1 << info.n);

    //PORT <- 0: sets pin to pullup resistor mode
    *info.port |=  (1 << info.n);
}

// Set up all pins in a pin set for reading
template <pin... Ps> void setup_input_pins(pin_set<Ps...>) {
    (pin_set_for_reading<Ps>(), ...);
}

template <class Layouts, pin RowPin, size_t RowN, pin... ColPins, size_t... ColNs>
void scan_columns(static_vector<array<keys::key,variadic_size<Layouts>::value>,16>& pressed,
                  uint8_t& mod_keys, pin_set<ColPins...>, index_sequence<ColNs...>) {
    power_pin<RowPin>();
    _delay_us(30);  // without this wait read unstable value.
    (test_key<RowPin, RowN, ColPins, ColNs>(pressed, mod_keys, Layouts{}), ...);
    unpower_pin<RowPin>();
}

template <class Layouts, pin... RowPins, size_t... RowIdxs, class... ColScans>
auto matrix_scan(uint8_t& mod_keys, pin_set<RowPins...>, index_sequence<RowIdxs...>, typelist<ColScans...>) {
    constexpr auto n_layers = variadic_size<Layouts>::value;
    static_vector<array<keys::key,n_layers>,16> pressed;

    (scan_columns<Layouts, RowPins, RowIdxs>(pressed, mod_keys,
                                            ColScans{}, sequence_for_vallist<ColScans>{}), ...);
    return pressed;
}


/*template <class Kbd, class... Rows>
void validate_layout(type<layout<Rows...>>) {
    static_assert(sizeof...(Rows) == variadic_size<Kbd::rows>(), "A layout has the wrong number of rows");
    }*/


template <class Kbd>
void validate_keyboard() {
//    (validate_layout<Kbd>(type<Kbd::layouts>), ...);
}


// Main firmware loop
template <class Kbd>
void run_firmware() {
    while (true) {
        using scan_set = scan_set_for<Kbd>;
        using rows = typename Kbd::rows;
        keyboard_modifier_keys = 0;
        auto pressed = matrix_scan<decltype(Kbd::layouts())> (keyboard_modifier_keys,
                                                              rows{}, sequence_for_vallist<rows>{}, scan_set{});

        auto layer = 0;

        for (auto&& k : pressed) {
            using namespace keys;
            if (k[0] >= lay1 && k[0] <= lay9) {
                layer = k[0] - lay1 + 1;
            }
        }

        static_vector<keys::key, 6> real_keys;

        for (auto&& key_map : pressed) {
            auto key = key_map[layer];

            if (is_modifier(key)) {
                keyboard_modifier_keys |= get_mod_mask(key);
            }
            else {
                real_keys.push_back(key);
            }
        }

        for (auto i = 0u; i < real_keys.size(); ++i) {
            keyboard_keys[i] = real_keys[i];
        }


        for (auto i = real_keys.size(); i < 6; ++i) {
            keyboard_keys[i] = 0;
        }

        usb_keyboard_send();
    }
}

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

int main() {
    CPU_PRESCALE(0);
    usb_init();
    while (!usb_configured()) /* wait */ ;
    _delay_ms(10000);

    //   validate_keyboard<keyboard_to_run>();
    setup_input_pins(keyboard_to_run::columns{});
    run_firmware<keyboard_to_run>();
}
