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
            row<tab, quot,comm,dot, p,   y,   f,   g,   c,   r,   l,   bspc, a>,
            row<lctl,a,   o,   e,   u,   i,   d,   h,   t,   n,   s,   ent>,
            row<lsft,scln,q,   j,   k,   x,   b,   m,   w,   v,   z,   del>,
            row<caps,lgui,esc,lalt, spc,      lay1,c, left,down,up,  righ>
            >,

        typelist<
            row<tab, one, two, thre,four,five,six, sevn,eigh,nine,zero,bspc, a>,
            row<lctl,a,   o,   e,   u,   i,   d,   h,   t,   n,   s,   ent>,
            row<lsft,scln,q,   j,   k,   x,   b,   m,   w,   v,   z,   del>,
            row<caps,lgui,esc,lalt, spc,      lay1,c, left,down,up,  righ>
            >
    >{};
}
