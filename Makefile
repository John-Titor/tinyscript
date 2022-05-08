
# for linenoise
READLINE=linenoise.o
READLINE_DEFS=-DLINENOISE

OPTS=-g -Og
CC=gcc
CFLAGS=	$(OPTS) \
	$(READLINE_DEFS) \
	-DTINYSCRIPT_VERBOSE_ERRORS \
	-DTINYSCRIPT_ARRAY_SUPPORT \
	-DTINYSCRIPT_FLOAT_SUPPORT \
	-Wall

OBJS=main.o tinyscript.o tinyscript_api.o tinyscript_math.o

tstest: $(OBJS) $(READLINE)
	$(CC) $(CFLAGS) -o tstest $(OBJS) $(READLINE)

clean:
	rm -f *.o

.PHONY: test
test: tstest
	(cd Test; ./runtests.sh)
