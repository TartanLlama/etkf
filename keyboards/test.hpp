#include "keys.hpp"
#include "type.hpp"

using namespace etkf;
using namespace etkf::pins;

struct test_keyboard {
    using rows = pin_set<f4, f1, f6, f0>;
    using columns = pin_set<d7, d6, c7, c6, d3, d2, d1, d0, b7, b3, b2, b1, b0>;

    auto layouts();

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
        row<tab, quot,comm,dot, p,   y,   f,   g,   c,   r,   l,   bspc, a>,
        row<lctl,a,   o,   e,   u,   i,   d,   h,   t,   n,   s,   ent>,
        row<lsft,scln,q,   j,   k,   x,   b,   m,   w,   v,   z,   del>,
        row<caps,lgui,esc,lalt, spc,      b,   c, left,down,up,  righ>
    >{};
}
