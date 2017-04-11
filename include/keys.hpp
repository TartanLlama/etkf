#ifndef etkf_keys_hpp
#define etkf_keys_hpp

namespace etkf {
    namespace keys {
        enum key {
            null = 0,

            a	 = 4,
            b	 = 5,
            c	 = 6,
            d	 = 7,
            e	 = 8,
            f	 = 9,
            g	 = 10,
            h	 = 11,
            i	 = 12,
            j	 = 13,
            k	 = 14,
            l	 = 15,
            m	 = 16,
            n	 = 17,
            o	 = 18,
            p	 = 19,
            q	 = 20,
            r	 = 21,
            s	 = 22,
            t	 = 23,
            u	 = 24,
            v	 = 25,
            w	 = 26,
            x	 = 27,
            y	 = 28,
            z	 = 29,
            one	 = 30,
            two	 = 31,
            thre	 = 32,
            four	 = 33,
            five	 = 34,
            six	 = 35,
            sevn	 = 36,
            eigh	 = 37,
            nine	 = 38,
            zero	 = 39,
            ent = 40,
            esc	 = 41,
            bspc = 42,
            tab	 = 43,
            spc = 44,
            min = 45,
            eql = 46,
            lbra = 47, //left brace
            rbra = 48,
            bsla = 49, //backslash
            num = 50,
            scln = 51, //semicolon
            quot = 52,
            tild = 53,
            comm = 54,
            dot = 55,
            slsh = 56,
            caps = 57, //caps lock
            f1	 = 58,
            f2	 = 59,
            f3	 = 60,
            f4	 = 61,
            f5	 = 62,
            f6	 = 63,
            f7	 = 64,
            f8	 = 65,
            f9	 = 66,
            f10	 = 67,
            f11	 = 68,
            f12	 = 69,
            pscr = 70, //print screen
            srlk = 71, //scroll lock
            paus = 72, //pause
            ins = 73,
            home = 74,
            pgup = 75,
            del = 76,
            end	 = 77,
            pgdn = 78,
            righ = 79, //right
            left = 80,
            down = 81,
            up	 = 82,
            numl = 83, //numlock
            ksla = 84,//keypad slash
            kast = 85, //keypad asterisk
            kmin = 86,
            kplu = 87,
            kent = 88,
            k1 = 89, //keypad 1
            k2 = 90,
            k3 = 91,
            k4 = 92,
            k5 = 93,
            k6 = 94,
            k7 = 95,
            k8 = 96,
            k9 = 97,
            k0 = 98,
            kdot = 99, //keypad dot

            lctl,
            lsft,
            lalt,
            lgui,
            rctl,
            rsft,
            ralt,
            rgui,

        };
    }

    inline bool is_modifier (keys::key k) {
        using namespace keys;

        switch (k) {
        case lctl:
        case lsft:
        case lalt:
        case lgui:
        case rctl:
        case rsft:
        case ralt:
        case rgui:
            return true;

        default:
            return false;
        }
    }


    inline uint8_t get_mod_mask (keys::key k) {
        using namespace keys;

        switch (k) {
        case lctl: return 0x01;
        case lsft: return 0x02;
        case lalt: return 0x04;
        case lgui: return 0x08;
        case rctl: return 0x10;
        case rsft: return 0x20;
        case ralt: return 0x40;
        case rgui: return 0x80;

        default:
            __builtin_unreachable();
        }
    }
}

#endif
