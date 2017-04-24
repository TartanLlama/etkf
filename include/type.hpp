
#ifndef etkf_type_hpp
#define etkf_type_hpp

namespace etkf {
    namespace pins {
        enum pin {
            b0, b1, b2, b3, b4, b5, b6, b7,
            c6, c7,
            d0, d1, d2, d3, d4, d5, d6, d7,
            e2, e6,
            f0, f1, f4, f5, f6, f7
        };

        enum registers {
            pinb = 0x03,
            ddrb = 0x04,
            portb = 0x05,

            pinc = 0x06,
            ddrc = 0x07,
            portc = 0x08,

            pind = 0x09,
            ddrd = 0x0a,
            portd = 0x0b,

            pine = 0x0c,
            ddre = 0x0d,
            porte = 0x0e,

            pinf = 0x0f,
            ddrf = 0x10,
            portf = 0x11
        };
    }

    template <class T> struct type{};
    template <class... T> struct typelist{};
    template <pins::pin... T> struct pin_set{};
    template <auto... T> struct row{};
}

#endif
