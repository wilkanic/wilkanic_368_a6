WARNING = -Wall -Wshadow --pedantic
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g $(WARNING) $(ERROR) 

TESTFLAGS = -DTEST_COUNTWORD

SRCS = a6_main.c
OBJS = $(SRCS:%.c=%.o)


a6: $(OBJS) 
	$(GCC) $(TESTFLAGS) $(OBJS) -o a6

.c.o: 
	$(GCC) $(TESTFLAGS) -c $*.c 

clean:
	rm -f a6 *.o output* *~
