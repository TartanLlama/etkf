#!/usr/bin/env python

import os
import argparse

parser = argparse.ArgumentParser(description='Create the necessary files and changes to add a new keyboard to etkf.')
parser.add_argument('name', metavar='name', type=str,
                    help='name for the keyboard')

args = parser.parse_args()

script_path = os.path.dirname(os.path.realpath(__file__))
keyboards_dir = os.path.join(script_path, os.pardir, 'keyboards')

cmake_path = os.path.join(keyboards_dir, "CMakeLists.txt")
print ("Adding subdirectory to {}".format(cmake_path))
with open(cmake_path, 'a') as cmake_file:
    cmake_file.write("add_subdirectory({})\n".format(args.name))

new_dir = os.path.join(keyboards_dir, args.name)
print ("Making directory {}".format(new_dir))
os.mkdir(new_dir)


config_path = os.path.join(new_dir, "config.hpp")
print ("Making config file at {}".format(config_path))

with open(config_path, 'w') as config_file:
    contents = """
#include "keys.hpp"
#include "type.hpp"

using namespace etkf;
using namespace etkf::pins;

struct {name} {{
    using rows = pin_set</*enter the pins which your rows are wired to here*/>;
    //Example: using rows = pin_set<b4, b6, f1, f0>;

    using columns = pin_set</*enter the pins which your columns are wired to here*/>;
    //Example: using columns = pin_set<f6, f7, c7, c6, d3, d2, d1, d0, b7, b3, b2, b1, b0>;

    //For each row in your keyboard, indicate which columns have keys wired to them
    using key_positions = typelist<
       //example: row<1,1,1,0,1,1,0,1,1,1,1,1,1>
       //1 means that there's a key there
       //0 means that there isn't
    >;

    //Just leave this alone
    static auto layouts();
}};


using keyboard_to_run = {name};

auto {name}::layouts() {{
    using namespace etkf::keys;

    return typelist<
        //Put all of your layer key layouts here
        //See include/keys.hpp for all the supported keys
        //Example:
        //typelist<
        //    row<tab, q,   w,   e,   r,   t,   y,   u,   i,   o,   p,   bspc, lay3>,
        //    row<lctl,a,   s,   d,   f,   g,   h,   j,   k,   l,   scln,ent>,
        //    row<lsft,z,   x,   c,   v,   b,   n,   m,   comm,dot, slsh,del>,
        //    row<caps,lgui,esc,lalt, spc,      lay1,lay3,   left,down,up,  righ>
        //>
    >{{}};
}}

""".format(name=args.name)

    config_file.write(contents)

keyboard_cmake_path = os.path.join(new_dir, "CMakeLists.txt")
print ("Making CMakeLists.txt file at {}".format(config_path))

with open(keyboard_cmake_path, 'w') as keyboard_cmake_file:
    keyboard_cmake_file.write("add_keyboard({})\n".format(args.name))

print ("Done!")
