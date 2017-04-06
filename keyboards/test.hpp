struct test_keyboard {
    using columns = pin_set<D4,D5,D6>;
    using rows = pin_set<B1,B5,C4>;

    using layouts = layouts<
        layout<
            row<a, b, c, d>,
            row<e, f, g, h>
            >,

        layout<
            row<a, b, c, d>,
            row<e, f, g, h>
            >
        >;
};

using keyboard_to_run = test_keyboard;
