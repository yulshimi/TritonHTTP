
CC=g++ -std=c++11
CFLAGS=-ggdb -Wall -Wextra -pedantic -Werror
DEPS = httpd.h httpParser.hpp httpFramer.hpp
SRCS = httpd.cpp httpParser.cpp httpFramer.cpp
MAIN_SRCS = main.c $(SRCS)
MAIN_OBJS = $(MAIN_SRCS:.c=.o)

default: httpd

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

httpd:    $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o httpd $(MAIN_OBJS) -lpthread

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f httpd *.o
