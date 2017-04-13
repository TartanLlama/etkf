#ifndef etkf_debug_hpp
#define etkf_debug_hpp

enum class debug {
    none, some, more, all
};

#ifdef DEBUG_ALL
constexpr debug debug_level = debug::all;
#elif DEBUG_MORE
constexpr debug debug_level = debug::more;
#elif DEBUG_SOME
constexpr debug debug_level = debug::some;
#elif DEBUG_NONE
constexpr debug debug_level = debug::none;
#else
constexpr debug debug_level = debug::none;
#endif


#ifdef WITHOUT_VALIDATIONS
constexpr bool run_validations = false;
#else
constexpr bool run_validations = true;
#endif

#endif
