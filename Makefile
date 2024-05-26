CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
ARG = arg

georgecc: $(OBJS)
	$(CC) -o georgecc $(OBJS) $(LDFLAGS)

$(OBJS): georgecc.h

test: georgecc.h
	./test.sh

# make check ARG="12+2;"
check: georgecc.h
	./test2.sh '${ARG}'

clean:
	rm -f georgecc *.o *~ tmp*

.PHONY: test clean
