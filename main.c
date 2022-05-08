#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#if defined(LINENOISE)
#include "linenoise.h"
#endif

#include "tinyscript_api.h"
#ifdef TINYSCRIPT_FLOAT_SUPPORT
#include "tinyscript_math.h"
#endif

#define ARENA_SIZE 8192
#define MAX_SCRIPT_SIZE 100000

int inchar() {
    return getchar();
}
void outchar(int c) {
    putchar(c);
}

#ifdef MAX_SCRIPT_SIZE
char script[MAX_SCRIPT_SIZE];

int
TinyScript_Stop(void)
{
    return 0;
}

void
runscript(const char *filename)
{
    FILE *f = fopen(filename, "r");
    int r;
    if (!f) {
        perror(filename);
        return;
    }
    r=fread(script, 1, MAX_SCRIPT_SIZE, f);
    fclose(f);
    if (r <= 0) {
        fprintf(stderr, "File read error on %s\n", filename);
        return;
    }
    script[r] = 0;
    r = TinyScript_RunMain(script);
    if (r != 0) {
        printf("script error %d\n", r);
    }
    exit(r);
}
#endif

// compute a function of two variables
// used for testing scripts
static intptr_t testfunc(intptr_t x, intptr_t y)
{
    return x*x + y*y;
}

struct def {
    const char *name;
    int nargs;
    void *val;
} funcdefs[] = {
    { "dsqr",      2, (void *)testfunc },
    { NULL, 0 }
};

void
REPL()
{
    int r;
    char *buf;
    
    linenoiseHistoryLoad("tinyscript_history");
    
    for(;;) {
#if defined(LINENOISE)        
        buf = linenoise("ts> ");
        if (!buf) break;
        linenoiseHistoryAdd(buf);
        linenoiseHistorySave("tinyscript_history");
#else
        static char sbuf[128];
        
        printf("ts> "); fflush(stdout);
        buf = fgets(sbuf, sizeof(sbuf), stdin);
        if (!buf) break;
#endif
        r = TinyScript_Eval(buf);
        if (r != 0) {
            printf("error %d\n", r);
        }
#if defined(LINENOISE)
        free(buf);
#endif        
    }
}

char memarena[ARENA_SIZE];

int
main(int argc, char **argv)
{
    int err;
    int i;
    
    err = TinyScript_InitInterp(memarena, sizeof(memarena));
    for (i = 0; funcdefs[i].name; i++) {
        err |= TinyScript_DefineCFunction(funcdefs[i].name, funcdefs[i].nargs, funcdefs[i].val);
    }
    if (err != 0) {
        printf("Initialization failed!\n");
        return 1;
    }
#ifdef TINYSCRIPT_FLOAT_SUPPORT
    err |= ts_define_math_funcs();
    if (err != 0) {
        printf("Initialization of math library failed!\n");
        return 1;
    }
#endif
    if (argc > 2) {
        printf("Usage: tinyscript [file]\n");
    }
    if (argv[1]) {
        runscript(argv[1]);
    } else {
        REPL();
    }
    return 0;
}

