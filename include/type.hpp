#ifndef etkf_type_hpp
#define etkf_type_hpp

namespace etkf {
    namespace pins {
        enum pin {
            f0, f1, f4, f5, f6, f7,
            b6, b5, b4, d7, d6, d4,
            d5, c6, c7, d3, d2, d1, b0,
            d0, b7, e6, b3, b2, b1,
        };
    }

    template <class T> struct type{};
    template <class... T> struct typelist{};
    template <pins::pin... T> struct pin_set{};
    template <int... T> struct row{};
}

#endif
