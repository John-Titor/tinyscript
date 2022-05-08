Introduction
============

This is tinyscript, a scripting language designed for very tiny
machines. The initial target is boards using the Parallax Propeller,
which has 32KB of RAM, but the code is written in ANSI C so it should
work on any platform (e.g. testing is done on x86-64 Linux).

On the propeller, the interpreter code needs about 3K of memory in CMM
mode or 5K in LMM. On the x86-64 the interpreter code is 6K. The size
of the workspace you give to the interpreter is up to you, although in
practice it would not be very useful to use less than 2K of RAM. The
processor stack is used as well, so it will need some space.

tinyscript is copyright 2016-2021 Total Spectrum Software Inc. and released
under the MIT license. See the COPYING file for details.

Modifications unique to this version are licensed CC0 (creative commons zero)
and may be freely used by anyone for any purpose. Documentation for these
modifications is marked ++ in the documentation below.

The Language
============
The scripting language itself is pretty minimalistic. The grammar for it
looks like:

    <program> ::= <stmt> | <stmt><sep><program>
    <stmt> ::= <vardecl> | <arrdecl> | <funcdecl> |
               | <assignment> | <ifstmt>
               | <whilestmt> | <funccall>
               | <printstmt> | <returnstmt>

The statements in a program are separated by newlines or ';'.

Either variables or functions may be declared.

    <vardecl> ::= "var" <assignment>
    <arrdecl> ::= "array" <symbol> "(" <number> ")" | "array" <symbol>
    <funcdecl> ::= "func" <symbol> "(" <varlist> ")" <string>
    <assignment> ::= <symbol> "=" <expr>
    <varlist> ::= <symbol> [ "," <symbol> ]+
    
Variables must always be given a value when declared (unless they are
arrays). All non-array variables simply hold 32 bit quantities,
normally interpreted as an integer.  The symbol in an assignment
outside of a vardecl must already have been declared.

Arrays are simple one dimensional arrays. Support for arrays does add
a little bit of code, so they are optional (included if TINYSCRIPT_ARRAY_SUPPORT
is defined in tinyscript.h). If the array declaration includes a size,
then a new (uninitialized) array is created. If it does not include a
size, then it must match one of the enclosing function's parameters,
in which case that parameter is checked and must be an array.

Array indices start at 0. Array index -1 is special and holds the
length of the array,

Functions point to a string. When a procedure is called, the string
is interpreted as a script (so at that time it is parsed using the
language grammar). If a function is never called then it is never
parsed, so it need not contain legal code if it is not called.

Strings may be enclosed either in double quotes or between { and }.
The latter case is more useful for functions and similar code uses,
since the brackets nest. Also note that it is legal for newlines to
appear in {} strings, but not in strings enclosed by ".

    <ifstmt> ::= "if" <expr> <string> [<elsepart>]
    <elsepart> ::= "else" <string> | "elseif" <expr> [<elsepart>]
    <whilestmt> ::= "while" <expr> <string> [<elsepart>]

As with functions, the strings in if and while statements are parsed
and interpreted on an as-needed basis. Any non-zero expression is
treated as true, and zero is treated as false. As a quirk of the
implementation, it is permitted to add an "else" clause to a while statement;
any such clause will always be executed after the loop exits.

    <returnstmt> ::= "return" <expr>

Return statements are used to terminate a function and return a value
to its caller.

    <printstmt> ::= "print" <printitem> [ "," <printitem>]+
    <printitem> ::= <string> | <expr>

Expressions are built up from symbols, numbers (decimal or hex integers), and
operators. The operators have precedence levels 1-4. Level 0 of expressions is
the most basic, consisting of numbers or variables optionally preceded by a
unary operator:

    <expr0> ::= <symbol> | <number> 
                | <unaryop><expr0> 
                | "(" <expr> ")"
                | <builtincall>
    <funccall> ::= <symbol> "(" [<exprlist>] ")"
    <exprlist> ::= <expr> ["," <expr>]*

    <number> ::= <digit>+ | "0x"<digit>+ | "'"<asciicharsequence>"'" | <digit>+"."<digit>*

    <asciicharsequence> ::= <printableasciichar> | "\'" | "\\" | "\n" | "\t" | "\r"

    <printableasciichar> ::= ' ' to '~' excluding ' and \

    <expr1> ::= <expr0> [<binop1> <expr0>]*
    <binop1> ::= "*" | "/" | "%"

    <expr2> ::= <expr1> [<binop2> <expr2>]*
    <binop2> ::= "+" | "-"

    <expr3> ::= <expr2> [<binop3><expr3>]*
    <binop3> ::= "&" | "|" | "^" | "<<" | ">>"

    <expr4> ::= <expr3> [<binop4><expr4>]*
    <binop4> ::= "==" | "<>" | ">" | "<" | ">=" | "<="

    <expr5> ::= <expr4> [<binop5><expr5>]*
    <binop5> ::= "&&"

    <expr6> ::= <expr5> [<binop6><expr6>]*
    <binop6> ::= "||"

    <unaryop> ::= <binop1> | <binop2> | <binop3> | <binop4> | <binop5> | <binop6> | "~" | "!"

Builtin functions are defined by the runtime, as are operators. The ones
listed above are merely the ones defined by default. Operators may use
any of the characters `!=<>+-*/&|^%~!`. Any string of the characters
`!=<>&|^~!` is processed together, but the operator characters `+-*/` may only
appear on their own.

Note that any operator may be used as a unary operator, and in this case
`<op>x` is interpreted as `0 <op> x` for any operator `<op>`. This is useful
for `+`, `-`, `~`, and `!`, less so for other operators.

`%` is the modulo operator, as in C. The `~` and `!` operators perform bitwise 
or logical inversion respectively, as a unary operator, and ignore the left-side 
argument if used as a binary operator.

`&&` and `||` are the boolean AND and OR operators respectively, again as in C. ++

Note: the equality operator `==` differs from regular tinyscript, which
confusingly uses `=` for both assignment and equality comparison. ++

Variable Scope
--------------

Variables are dynamically scoped. For example, in:
```
var x=2

func printx() {
  print x
}
func myfunc() {
  var x=3
  printx()
}
```
invoking `myfunc` will cause 3 to be printed, not 2 as in statically scoped
languages.

Floating Point ++
-----------------

Floating point values may also be stored in variables. Support for 
floating point may add considerable code, so it is optional (included
if TINYSCRIPT_FLOAT_SUPPORT is defined in tinyscript.h). 

Variables have no type information, so it is necessary to explicitly convert
between floating point and integer values. It can be helpful to prefix
floating-point variable names with 'f' as a type hint.

Decimal constants containing a decimal point are automatically converted to
float in a somewhat imprecise fashion. If precision is important, use explicit
hexadecimal constants. The math library (see below) defines the usual <math.h>
constants.
```
var fX = float(10)
var fY = fsub(0, 21.4)
var fPI = 3.14159265358979323846
print "PI is approximately ",int(fPI)
print "manual PI ",fPI
print "library PI ",M_PI
```

The standard arithmetic operators interpret their arguments as integers, so
the builtin functions `fadd`, `fsub`, `fmul`, and `fdiv` should be used for
floating point values.

Likewise the unary `-` operator cannot negate a floating point number; use
`fneg(fVal)` instead.

Comparisons can be made using `fgt` and `flt`. Equality comparison
for floating point numbers is tricky and should be avoided, but see `fdim` in
the math library.
```
var area = fmul(fPI, fmul(fX, fX))
```

Builtins also exist for the basic classification functions `isfinite`, 
`isinf`, and `isnan`.
```
var fY = fdiv(10.0, 0.0)
if isinf(fY) { print "oops" }
```

Interface to C
==============

Note ++
-------
The C interface is heavily modified in this implementation. 


Environment Requirements ++
---------------------------

The interpreter is quite self-contained. The functions needed to interface with
it are `putchar` (called to print a single character), `memcpy`, and
TinyScript_Stop(). 

Both `putchar` and `memcpy` can be supplied by the C runtime. TinyScript_Stop
() is called by the interpreter to check whether a running script should stop.


Configuration
-------------

Language configuration options should be passed in by the build system:

```
TINYSCRIPT_ARENA_SIZE     - sets the size of the memory area reserved for TinyScript
TINYSCRIPT_VERBOSE_ERRORS - gives better error messages (costs a tiny bit of space)
TINYSCRIPT_ARRAY_SUPPORT  - include support for integer arrays
TINYSCRIPT_FLOAT_SUPPORT  - include support for single-precision floating point
```

Math Library ++
---------------

There is an optional floating point math library in tinyscript_math.{c,h} 
that exposes a subset of the <math.h> functions and constants. This adds a 
dependency on TINYSCRIPT_FLOAT_SUPPORT.


API ++
------

See the documentation in `tinyscript_api.h`


Math Library ++
---------------
The math library is optional, and is found in the file `tinyscript_math.c`. It must be
initialized with `ts_define_math_funcs()` before use. Functions provided are:

`signbit(x)`: returns 1 if x < 0, 0 otherwise

`ceil(x)`: returns the smallest integral value greater than or equal to x

`floor(x)`: returns the largest integral value less than or equal to x

`round(x)`: returns the integral value nearest to x rounding half-way cases away from zero

`trunc(x)`: returns the integral value nearest to but no larger in magnitude than x

`fmod(x, y)`: returns the floating-point remainder of x / y

`remainder(x, y)`: returns the value r such that r = x - n * y, where n is the integer nearest the exact value of x/y

`fdim(x, y)`: returns the "positive difference" between the arguments:  x - y if x > y, +0 if x is less than or equal to y

`fmin(x, y)`: returns x or y, whichever is smaller

`fmax(x, y)`: returns x or y, whichever is larger

`fabs(x)`: returns the absolute value of x

`sqrt(x)`: returns the non-negative square root of x

`pow(x, y)`: returns x raised to the power y

For more detailed descriptions of these functions, consult the documentation for
the platform's math library.

Acknowledgements
================
I'd like to thank Mickey Delp and Daniel Landau for their contributions to tinyscript. Daniel's bug reports have been invaluable, and he contributed (among other things) the optional standard library, readline support, and hex support. Mickey contributed better error handling, the modulo operator, and optional array support.

