#ifndef etkf_list_utils_hpp
#define etkf_list_utils_hpp

namespace etkf {
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
}

#endif
