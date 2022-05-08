/*
 * Public tinyscript API
 *
 * Functions return 0 on success.
 */

#pragma once
#include <stdint.h>

/*
 * (Re)initialize the TinyScript interpreter.
 */
extern int TinyScript_InitInterp();

/*
 * Run a script at the top level.
 *
 * The interpreter will reference strings in the script, so it
 * must not be overwritten while the interpreter state remains.
 */
extern int TinyScript_RunMain(const char *script);

/*
 * Run a nested / callback script.
 *
 * Use this to run a script or callback when the interpreter
 * has called out to a C function. The callback script will run
 * in the context of the function that called out.
 *
 * The interpreter will reference strings in the script, so it
 * must not be overwritten while the interpreter state remains.
 */
extern int TinyScript_RunCallback(const char *script);

/*
 * Run a script or fragment from a transient buffer at the top 
 * level.
 *
 * The interpreter will make a copy of all strings in the script,
 * so it may be overwritten once evaluation completes.
 */
extern int TinyScript_Eval(const char *script);

/*
 * Run a script or fragment from a transient buffer as a nested
 * or callback script.
 *
 * Use this to run a dynamically-generated callback in the context
 * of the script function that called out.
 *
 * The interpreter will make a copy of all strings in the script,
 * so it may be overwritten once evaluation completes.
 */
extern int TinyScript_EvalCallback(const char *script);

/*
 * Set a variable.
 *
 * If the variable already exists, its value will be updated. This may
 * reach back up the call stack from the current scope.
 *
 * If the variable does not exist, it will be created at the current scope.
 * If a script is not running, this will be at global scope and it will
 * persist until the interpreter is re-initialized.
 *
 * Attempting to overrun the end of an array will return an error. Only
 * partially updating an array will not. Array sizes cannot be changed.
 *
 * If a new variable is being created, the name argument will be
 * referenced by the interpreter, and must not be overwritten while
 * the interpreter state remains.
 *
 * Note: arrays can only be set, not created. Use TinyScript_Run* or
 *       TinyScript_Eval* to create arrays.
 */
extern int TinyScript_SetInt(const char *name, int value);
extern int TinyScript_SetFloat(const char *name, float value);
extern int TinyScript_SetArray(const char *name, int len, int *values);

/*
 * Get a variable or array.
 *
 * Note that GetArray returns a reference to the array values; this
 * pointer is only valid until the script is resumed.
 *
 * The first value in an array is always the array limit, i.e. an array
 * declared 'array foo(3)' will look like 'intptr foo[4]' where 
 * 'foo[0] == 3'.
 */
extern int TinyScript_GetInt(const char *name, int *value);
extern int TinyScript_GetFloat(const char *name, float *value);
extern int TinyScript_GetArray(const char *name, int *len, int **values);

/*
 * Add a new builtin function or operator.
 *
 * Note that functions added while a script is running will be added
 * at the current script scope. Returning from that scope will clear the
 * definition, which is probably undesirable.
 *
 * Functions take 0-4 intptr_t arguments, which may be integers, floating
 * point values type-punned over integers, or pointers to arrays. Use
 * TinyScript_CheckArray() to validate array arguments.
 *
 * Functions must return a value; either an intptr_t or a floating point value
 * type-punned over an intptr_t. Arrays cannot be returned directly, but as they
 * are always passed by reference, they can be updated by the called function.
 *
 * Operator functions always receive two arguments; the left and right side
 * respectively, and return the result of the operation. If used as a unary
 * operator, the left-side argument will be zero.
 */
extern int TinyScript_DefineCFunction(const char *name, int num_args, void *func);
extern int TinyScript_DefineOperator(const char *name, int precedence, void *func);


/*
 * Verify that a C function argument can be treated as an array.
 *
 * Note that this doesn't verify that it *is* an array, just that 
 * treating it like one won't reach outside of the arena.
 *
 * Returns zero if the array is not invalid.
 */
extern int TinyScript_CheckArray(intptr_t array);
