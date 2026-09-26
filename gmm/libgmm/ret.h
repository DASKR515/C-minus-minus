// doc ret.h = https://github.com/DASKR515/C-minus-minus/blob/main/stdc--.h/RET.md

#ifndef CMM_RET_H
#define CMM_RET_H

#define fnc(name) \
    name (W_ ret_addr)

#define fnc_a(name, ...) \
    name (W_ ret_addr, __VA_ARGS__)

#define ret_v() \
    jump ret_addr ()

#define ret(val) \
    jump ret_addr (val)

#define ret2(v1, v2) \
    jump ret_addr (v1, v2)

#define ret3(v1, v2, v3) \
    jump ret_addr (v1, v2, v3)

#define mall_v(fn, cont) \
    jump fn (cont)

#define mall(fn, cont, ...) \
    jump fn (cont, __VA_ARGS__)

#endif
