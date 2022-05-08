/*
 * Tinyscript API wrapper
 */

#include <stdbool.h>

#include "tinyscript_api.h"
#include "tinyscript_internal.h"
#include "tinyscript_math.h"

#ifndef TINYSCRIPT_ARENA_SIZE
#define TINYSCRIPT_ARENA_SIZE   4096
#endif

#define TINYSCRIPT_ARENA_WORDS  (TINYSCRIPT_ARENA_SIZE / 4)
static intptr_t ts_arena[TINYSCRIPT_ARENA_WORDS] = {0};   // XXX must be aligned!

int
TinyScript_InitInterp()
{
    int err = 0;

    err |= TinyScript_Init(ts_arena, sizeof(ts_arena));
#ifdef TINYSCRIPT_FLOAT_SUPPORT
    err |= ts_define_math_funcs();
#endif
    return err;
}

int
TinyScript_RunMain(const char *script)
{
    return TinyScript_Run(script, 0, 1);
}

int
TinyScript_RunCallback(const char *script)
{
    return TinyScript_Run(script, 0, 0);
}

int
TinyScript_Eval(const char *script)
{
    return TinyScript_Run(script, 1, 1);
}

int
TinyScript_EvalCallback(const char *script)
{
    return TinyScript_Run(script, 1, 0);
}

int
TinyScript_SetInt(const char *name, int value) 
{
    FloatVal fv = { .val = value };
    if (TinyScript_Set(name, INT, fv.val) == TS_ERR_OK) return TS_ERR_OK;
	return TinyScript_Define(name, INT, fv.val);
}

int
TinyScript_SetFloat(const char *name, float value) 
{
    FloatVal fv = { .flt = value };
    if (TinyScript_Set(name, INT, fv.val) == TS_ERR_OK) return TS_ERR_OK;
    return TinyScript_Define(name, INT, fv.val);
}

int
TinyScript_SetArray(const char *name, int len, int *values)
{
    int existing_len;
    int *existing_values;
    int err;

    err = TinyScript_GetArray(name, &existing_len, &existing_values);
    if (err == TS_ERR_OK) {
        if (len > existing_len) {
            err = TS_ERR_OUTOFBOUNDS;
        } else {
            for (int i = 0; i < len; i++) {
                existing_values[i] = values[i];
            }
        }
    }
    return err;
}

int
TinyScript_GetInt(const char *name, int *value)
{
    Val v;

    int err = TinyScript_Get(name, INT, &v);
    if (err == TS_ERR_OK) {
        *value = v;
    }
    return err;
}

int
TinyScript_GetArray(const char *name, int *len, int **values)
{
    intptr_t ary;

    int err = TinyScript_Get(name, ARRAY, &ary);
    if (err == TS_ERR_OK) {
        *len = *(int *)ary;
        *values = (int *)ary + 1;
    }
    return err;
}

int
TinyScript_GetFloat(const char *name, float *value)
{
    FloatVal fv;
    Val v;

    int err = TinyScript_Get(name, INT, &v);
    if (err == TS_ERR_OK) {
        fv.val = v;
        *value = fv.flt;
    }
    return err;
}

int
TinyScript_DefineCFunction(const char *name, int num_args, void *func) 
{
    if (num_args > 4) {
        return TS_ERR_TOOMANYARGS;
    }
	return TinyScript_Define(name, CFUNC(num_args), (intptr_t)func);
}

int
TinyScript_DefineOperator(const char *name, int precedence, void *func) 
{
    if (precedence >= MAX_EXPR_LEVEL) {
        return TS_ERR_BADARGS;
    }
    return TinyScript_Define(name, BINOP(precedence), (intptr_t)func);
}

int
TinyScript_CheckArray(intptr_t array)
{
    // must be aligned
    if (array & (sizeof(intptr_t) - 1)) {
        return 0;
    }

    // length is inside the arena?
    // XXX should use valptr as lower bound to ensure array still in scope...
    // 
    const intptr_t * const ary = (const intptr_t *)array;
    const intptr_t * const arena_limit = &ts_arena[TINYSCRIPT_ARENA_WORDS - 1];
    if ((ary < &ts_arena[0]) || (ary >= arena_limit)) {
        return 0;
    }

    // length is sane?
    intptr_t ary_len = ary[0];
    if ((ary_len <= 0) || (ary_len >= TINYSCRIPT_ARENA_WORDS)) {
        return 0;
    }

    // array doesn't reach outside the arena?
    if ((ary + ary_len + 1) >= arena_limit) {
        return 0;
    }
    return 1;
}
