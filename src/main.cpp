#include <stddef.h>
#include <stdint.h>
#include <avr/io.h>

#include "print.h"
#include <util/delay.h>

//This file will be provided by the build system and gives the keyboard configuration
#include "config.hpp"

#include "debug.hpp"
#include "keys.hpp"
#include "array.hpp"
#include "index_sequence.hpp"
#include "list_utils.hpp"
#include "static_vector.hpp"
#include "validations.hpp"

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
    uint32_t pin;
    uint32_t ddr;
    uint32_t port;
    int n;
};

template <uint32_t R>
volatile uint8_t& get_mem() {
    return *reinterpret_cast<volatile uint8_t*>(R + __SFR_OFFSET);
}

template <pin P>
constexpr pin_info get_pin_info() {
    using namespace pins;

    constexpr pin_info map[] = {
        { pinb, ddrb, portb, 0 },
        { pinb, ddrb, portb, 1 },
        { pinb, ddrb, portb, 2 },
        { pinb, ddrb, portb, 3 },
        { pinb, ddrb, portb, 4 },
        { pinb, ddrb, portb, 5 },
        { pinb, ddrb, portb, 6 },
        { pinb, ddrb, portb, 7 },
        { pinc, ddrc, portc, 6 },
        { pinc, ddrc, portc, 7 },
        { pind, ddrd, portd, 0 },
        { pind, ddrd, portd, 1 },
        { pind, ddrd, portd, 2 },
        { pind, ddrd, portd, 3 },
        { pind, ddrd, portd, 4 },
        { pind, ddrd, portd, 5 },
        { pind, ddrd, portd, 6 },
        { pind, ddrd, portd, 7 },
        { pine, ddre, porte, 2 },
        { pine, ddre, porte, 6 },
        { pinf, ddrf, portf, 0 },
        { pinf, ddrf, portf, 1 },
        { pinf, ddrf, portf, 4 },
        { pinf, ddrf, portf, 5 },
        { pinf, ddrf, portf, 6 },
        { pinf, ddrf, portf, 7 }
    };

    return map[P];
}

template <pin RowPin, size_t RowN, pin ColPin, size_t ColN, class... Layouts>
void test_key (static_vector<array<keys::key,sizeof...(Layouts)>,16>& pressed,
               uint8_t& mod_keys, typelist<Layouts...>) {
    constexpr auto info = get_pin_info<ColPin>();

    if (!(get_mem<info.pin>() & (1 << info.n))) {
        array<keys::key,sizeof...(Layouts)> possible_keys {
            index_vallist<typename index_typelist<Layouts, RowN>::type, ColN>::value...
        };
        pressed.push_back(possible_keys);
    }
}

// Power a pin so that keys on that line can be tested
template <pin P> void power_pin() {
    constexpr auto info = get_pin_info<P>();

    //DDR <- 1: sets pin to output mode
    get_mem<info.ddr>()  |=  (1 << info.n);

    //PORT <- 0: sets pin to low output
    get_mem<info.port>() &= ~(1 << info.n);
}

// Stop powering a pin
template <pin P> void unpower_pin() {
    constexpr auto info = get_pin_info<P>();

    //DDR <- 0: sets pin to input mode
    get_mem<info.ddr>()  &= ~(1 << info.n);

    //PORT <- 0: sets pin to normal mode
    get_mem<info.port>() &= ~(1 << info.n);
}

// Set up a pin for reading at a later time
template <pin P> void pin_set_for_reading() {
    constexpr auto info = get_pin_info<P>();

    //DDR <- 0: sets pin to input mode
    get_mem<info.ddr>()  &= ~(1 << info.n);

    //PORT <- 0: sets pin to pullup resistor mode
    get_mem<info.port>() |=  (1 << info.n);
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


// Main firmware loop
template <class Kbd>
void run_firmware() {
    auto base_layer = 0;

    while (true) {
        using scan_set = scan_set_for<Kbd>;
        using rows = typename Kbd::rows;
        keyboard_modifier_keys = 0;
        auto pressed = matrix_scan<decltype(Kbd::layouts())> (keyboard_modifier_keys,
                                                              rows{}, sequence_for_vallist<rows>{}, scan_set{});

        auto layer = base_layer;

        //First check if a layer key is pressed down
        for (auto&& k : pressed) {
            using namespace keys;
            if (k[0] >= lay0 && k[0] <= lay9) {
                layer = k[0] - lay0;
            }
        }

        //Then check if we need to change base layer
        for (auto&& k : pressed) {
            using namespace keys;
            if (k[layer] >= swi0 && k[layer] <= swi9) {
                base_layer = k[layer] - swi0;
            }
        }

        static_vector<keys::key, 6> real_keys;

        //Finally lookup the correct key to send by using the correct layer
        for (auto&& key_map : pressed) {
            auto key = key_map[layer];

            if (is_modifier(key)) {
                keyboard_modifier_keys |= get_mod_mask(key);
            }
            else {
                real_keys.push_back(key);
            }
        }

        //Write the keys into the usb send buffer
        for (auto i = 0u; i < real_keys.size(); ++i) {
            keyboard_keys[i] = real_keys[i];
        }

        //0 out the rest so that we aren't sending old keys
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

    if constexpr (run_validations) {
        validate_keyboard<keyboard_to_run>();
    }

    setup_input_pins(keyboard_to_run::columns{});
    run_firmware<keyboard_to_run>();
}
