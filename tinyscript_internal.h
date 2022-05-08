#pragma once

#include <stdint.h>

// language configuration options

// define TINYSCRIPT_VERBOSE_ERRORS to get nicer error messages at a small cost in space
// costs about 500 bytes on the Propeller
//#define TINYSCRIPT_VERBOSE_ERRORS

// define TINYSCRIPT_ARRAY_SUPPORT to get support for integer arrays
// costs about 1K on the Propeller 
//#define TINYSCRIPT_ARRAY_SUPPORT

// define TINYSCRIPT_FLOAT_SUPPORT to get support for floating point
// literals, casts to/from integers, and basic math operations
// on floating point values.
//#define TINYSCRIPT_FLOAT_SUPPORT

// Comment this out if you have provided a function to
// check whether a running script should stop. This
// function should return non-zero when if the script
// should stop, 0 if not.
//#define TinyScript_Stop() (0)

// errors
// all the ParseXXX functions return 0 on success, a negative
// error code otherwise
enum {
    TS_ERR_OK = 0,
    TS_ERR_NOMEM = -1,
    TS_ERR_SYNTAX = -2,
    TS_ERR_UNKNOWN_SYM = -3,
    TS_ERR_BADARGS = -4,
    TS_ERR_TOOMANYARGS = -5,
    TS_ERR_OUTOFBOUNDS = -6,
	TS_ERR_STOPPED = -7,
    TS_ERR_OK_ELSE = 1, // special internal condition
};

// Maximum expression precedence level
#define MAX_EXPR_LEVEL 7

// we use this a lot
typedef char Byte;

//our target is machines with < 64KB of memory, so 16 bit pointers
//will do
typedef Byte *Ptr;

// strings are represented as (length,ptr) pairs
// this is done so that we can re-use variable names and similar
// substrings directly from the script text, without having
// to insert 0 into them
typedef struct {
    unsigned len_;
    const char *ptr_;
} String;

// val has to be able to hold a pointer
typedef intptr_t Val;

#ifdef TINYSCRIPT_FLOAT_SUPPORT
// Use int32_t here to avoid issues testing on 64bit architectures where 
// intptr_t is too large.
typedef union { int32_t val; float flt; } FloatVal;
#endif

static inline unsigned StringGetLen(String s) { return (unsigned)s.len_; }
static inline const char *StringGetPtr(String s) { return (const char *)(intptr_t)s.ptr_; }
static inline void StringSetLen(String *s, unsigned len) { s->len_ = len; }
static inline void StringSetPtr(String *s, const char *ptr) { s->ptr_ = ptr; }

// symbols can take the following forms:
#define INT      0x0  // integer
#define STRING   0x1  // string
#define OPERATOR 0x2  // operator; precedence in high 8 bits
#define ARG      0x3  // argument; value is offset on stack
#ifdef TINYSCRIPT_ARRAY_SUPPORT
#define ARRAY    0x4  // integer array
#endif
#define BUILTIN  'B'  // builtin: number of operands in high 8 bits
#define USRFUNC  'f'  // user defined a procedure; number of operands in high 8 bits
#define TOK_BINOP 'o'

#define BINOP(x) (((x)<<8)+TOK_BINOP)
#define CFUNC(x) (((x)<<8)+BUILTIN)

typedef struct symbol {
    String name;
    int    type;   // symbol type
    Val    value;  // symbol value, or string ptr
} Sym;

#define MAX_BUILTIN_PARAMS 4

typedef Val (*Cfunc)(Val, Val, Val, Val);
typedef Val (*Opfunc)(Val, Val);

// structure to describe a user function
typedef struct ufunc {
    String body; // pointer to the body of the function
    int nargs;   // number of args
    // names of arguments
    String argName[MAX_BUILTIN_PARAMS];
} UserFunc;

//
// global interface
//
extern int TinyScript_Init(void *mem, int mem_size);
extern int TinyScript_Define(const char *name, int toktype, Val value);
extern int TinyScript_Run(const char *s, int saveStrings, int topLevel);
extern int TinyScript_Set(const char *s, int toktype, Val value);
extern int TinyScript_Get(const char *s, int toktype, Val *value);

// if an external function is provided, comment out the define, and uncomment the declaration
extern int TinyScript_Stop(void);
