template <class Kbd>
class firmware {
    void run();
    void matrix_scan();
};

template <class Kbd>
void firmware<Kbd>::run() {
    matrix_scan();
}

template <class Kbd>
void firmware<Kbd>::matrix_scan() {
    
}

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
