// This file contains a bunch of helpers for working with type and value parameter pack lists

#ifndef etkf_list_utils_hpp
#define etkf_list_utils_hpp

#include "index_sequence.hpp"

namespace etkf {
    // Given a typelist and an index, stores the Nth type in the list as a member alias
    // Assumes that the list is long enough
    template <class List, size_t N>
    struct index_typelist;

    // Recursive case
    template <template <class...> class List, class T, class... Ts, size_t N>
    struct index_typelist<List<T,Ts...>, N> {
        using type = typename index_typelist<List<Ts...>, N-1>::type;
    };

    // Found the type
    template <template <class...> class List, class T, class... Ts>
    struct index_typelist<List<T,Ts...>, 0> {
        using type = T;
    };

    // Given a list of non-type template params and an index, stores the Nth type in the list as static data member
    // Assumes that the list is long enough
    template <class List, size_t N>
    struct index_vallist;

    // Recursive case
    template <template <auto...> class List, auto T, auto... Ts, size_t N>
    struct index_vallist<List<T,Ts...>, N> {
        static constexpr auto value = index_vallist<List<Ts...>, N-1>::value;
    };

    // Found the value
    template <template <auto...> class List, auto T, auto... Ts>
    struct index_vallist<List<T,Ts...>, 0> {
        static constexpr auto value = T;
    };

    // Concatenates two typelists
    template <class List1, class List2>
    struct cat_typelist;

    template <template <auto...> class List, auto... Ts, auto... Us>
    struct cat_typelist<List<Ts...>, List<Us...>> {
        using type = List<Ts...,Us...>;
    };

    // Creates an index_sequence for a vallist
    template <class List> struct sequence_for_vallist_impl;

    template <template <auto...> class List, auto... Vals>
    struct sequence_for_vallist_impl<List<Vals...>> {
        using type = make_index_sequence<sizeof...(Vals)>;
    };

    template<class List>
    using sequence_for_vallist = typename sequence_for_vallist_impl<List>::type;

    template <class List>
    struct variadic_size;

    template <template <class...> class List, class... Ts>
    struct variadic_size<List<Ts...>> {
        static constexpr auto value = sizeof...(Ts);
    };

    template <template <auto...> class List, auto... Ts>
    struct variadic_size<List<Ts...>> {
        static constexpr auto value = sizeof...(Ts);
    };
}

#endif
