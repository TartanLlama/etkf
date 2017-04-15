# etkf
Embarrassingly templated keyboard framework

The aim of this is to have very terse descriptions of keyboards and give good compile-time diagnostics when mistaeks are made. Here's the entire configuration file for a simple 40% Dvorak keyboard:

```

#include "keys.hpp"
#include "type.hpp"

using namespace etkf;
using namespace etkf::pins;

struct test_keyboard {
    using rows = pin_set<b4, b6, f1, f0>;
    using columns = pin_set<f6, f7, c7, c6, d3, d2, d1, d0, b7, b3, b2, b1, b0>;

    static auto layouts();

    using key_positions = typelist<
        row<1,1,1,1,1,1,1,1,1,1,1,1,1>,
        row<1,1,1,1,1,1,1,1,1,1,1,0,1>,
        row<1,0,1,1,1,1,1,1,1,1,1,1,1>,
        row<1,1,1,0,1,1,0,1,1,1,1,1,1>
    >;

};

using keyboard_to_run = test_keyboard;

auto test_keyboard::layouts() {
    using namespace etkf::keys;

    return typelist<
        typelist<
            row<tab, quot,comm,dot, p,   y,   f,   g,   c,   r,   l,   bspc, null>,
            row<lctl,a,   o,   e,   u,   i,   d,   h,   t,   n,   s,   ent>,
            row<lsft,scln,q,   j,   k,   x,   b,   m,   w,   v,   z,   del>,
            row<caps,lgui,esc,lalt, spc,      null,null,left,down,up,  righ>
            >
    >{};
}
```

### How to add a new keyboard:

There is a Python script which will do all of the legwork for you:

    tools/add_new_keyboard.py <keyboard name>

### Current state:

- A configuration like the above will allow you to type things!
- Supports Teensy 2 (ATMEGA32U4)
- 6 key rollover
- Modifier keys
- Basic layer support
- Change default layer (e.g. QWERTY to Dvorak)

### To do:

- Debouncing
- NKRO
- Better compile-time diagnostics
- Debugging support
- Rewrite of the USB code
- Support more chips
- Port some keyboards
- Profile-driven optimisations

Dependencies:

- C++17 AVR compiler (you can build one by following [these instructions](http://www.atmel.com/webdoc/AVRLibcReferenceManual/install_tools.html))
- CMake
- Hardware to run it on
- Nachos (optional)
