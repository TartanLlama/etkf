#ifndef etkf_validations_hpp
#define etkf_validations_hpp

#include "type.hpp"

template <class Kbd, class... Rows>
void validate_layout(typelist<Rows...>) {
    static_assert(sizeof...(Rows) == variadic_size<typename Kbd::rows>::value,
                  "A layout has the wrong number of rows");
}

template <class Kbd, class... Layouts>
void validate_layouts(typelist<Layouts...>) {
    (validate_layout<Kbd>(Layouts{}), ...);
}

template <class Kbd>
void validate_keyboard() {
    validate_layouts<Kbd>(decltype(Kbd::layouts()){});
}

#endif
