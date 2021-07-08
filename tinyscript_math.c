/*
 * Floating point math library for Tinyscript.
 */

#include <math.h>

#include "tinyscript_math.h"

/* function taking float and returning Val */
#define FUNC_F(_name, _func)                \
    static Val _name (Val val) {            \
        FloatVal fv = { val };              \
        return _func (fv.flt);              \
    } struct hack

/* function taking float and returning float */
#define FUNC_F_F(_name, _func)              \
    static Val _name (Val val) {            \
        FloatVal fv = { val };              \
        fv.flt = _func(fv.flt);             \
        return fv.val;                      \
    } struct hack

/* function taking float, float and returning float */
#define FUNC_FF_F(_name, _func)             \
    static Val _name (Val val1, Val val2) { \
        FloatVal fv1 = { val1 };            \
        FloatVal fv2 = { val2 };            \
        fv1.flt = _func (fv1.flt, fv2.flt); \
        return fv1.val;                     \
    } struct hack

/*
 * Classification functions
 */
/* fpclassify - not implemented */
/* isfinite - base float support */
/* isinf - base float support */
/* isnan - base float support */
/* isnormal - not implemented */
FUNC_F(ts_signbit, signbit);

/* copysign - not implemented */
/* nextafter - not implemented */
/* nan - not implemented */

/*
 * Rounding functions
 */
FUNC_F_F(ts_ceil, ceilf);
FUNC_F_F(ts_floor, floorf);
/* nearbyint - not implemented */
/* rint - not implemented */
FUNC_F_F(ts_round, roundf);
FUNC_F_F(ts_trunc, truncf);

/*
 * remainder / modulus
 */
FUNC_FF_F(ts_fmod, fmod);
FUNC_FF_F(ts_remainder, remainderf);
/* remquo - not implemented */

/*
 * Delta/min/max
 */
FUNC_FF_F(ts_fdim, fdim);
FUNC_FF_F(ts_fmin, fmin);
FUNC_FF_F(ts_fmax, fmax);
/* fma - not implemented */

/*
 * Assorted math
 */
FUNC_F_F(ts_fabs, fabsf);
FUNC_F_F(ts_sqrt, sqrtf);
/* cbrt - not implemented */
/* hypot - not implemented */
FUNC_FF_F(ts_pow, powf);

static const struct {
    const char *name;
    float value;
} constants[] = {
    { "M_E",        M_E },
    { "M_LOG2E",    M_LOG2E },
    { "M_LOG10E",   M_LOG10E },
    { "M_LN2",      M_LN2 },
    { "M_LN10",     M_LN10 },
    { "M_PI",       M_PI },
    { "M_PI_2",     M_PI_2 },
    { "M_PI_4",     M_PI_4 },
    { "M_1_PI",     M_1_PI },
    { "M_2_PI",     M_2_PI },
    { "M_2_SQRTPI", M_2_SQRTPI },
    { "M_SQRT2",    M_SQRT2 },
    { "M_SQRT1_2",  M_SQRT1_2 },
};

int ts_define_math_funcs() {
    int err = 0, i;
    FloatVal fv;

    err |= TinyScript_Define("signbit", CFUNC(1), (Val)ts_signbit);

    err |= TinyScript_Define("ceil", CFUNC(1), (Val)ts_ceil);
    err |= TinyScript_Define("floor", CFUNC(1), (Val)ts_floor);
    err |= TinyScript_Define("round", CFUNC(1), (Val)ts_round);
    err |= TinyScript_Define("trunc", CFUNC(1), (Val)ts_trunc);

    err |= TinyScript_Define("fmod", CFUNC(2), (Val)ts_fmod);
    err |= TinyScript_Define("remainder", CFUNC(2), (Val)ts_remainder);

    err |= TinyScript_Define("fdim", CFUNC(2), (Val)ts_fdim);
    err |= TinyScript_Define("fmin", CFUNC(2), (Val)ts_fmin);
    err |= TinyScript_Define("fmax", CFUNC(2), (Val)ts_fmax);

    err |= TinyScript_Define("fabs", CFUNC(1), (Val)ts_fabs);
    err |= TinyScript_Define("sqrt", CFUNC(1), (Val)ts_sqrt);
    err |= TinyScript_Define("pow", CFUNC(2), (Val)ts_pow);

    // magic numbers
    for (i = 0; i < (sizeof(constants) / sizeof(constants[0])); i++) {
        fv.flt = constants[i].value;
        err |= TinyScript_Define(constants[i].name, INT, fv.val);
    }

    return err;
}
