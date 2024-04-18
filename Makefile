CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
ARG = arg

9cc: $(OBJS)
	$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

test: 9cc
	./test.sh

# make check ARG="12+2;"
check: 9cc
	bash -x ./test2.sh ${ARG}

clean:
	rm -f 9cc *.o *~ tmp*

.PHONY: test clean
