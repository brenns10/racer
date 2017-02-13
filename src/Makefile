CC=gcc
LIBS=ncurses
CFLAGS = $(shell pkg-config --cflags $(LIBS))
CFLAGS += -Wall -Wextra -pedantic -std=gnu11
LDLIBS = $(shell pkg-config --libs $(LIBS))

racer: racer.o window.o
