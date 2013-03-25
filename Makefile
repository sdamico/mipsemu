CC = gcc

CFLAGS = -Wall -std=gnu99 -O2
SOURCES = main.c mips.c font.c coetobin.c
HEADERS = mips.h font.h
OBJECTS = $(SOURCES:.c=.o)

TARGETS = coetobin mipsemu

all: $(TARGETS)

coetobin: coetobin.o
	$(CC) $(CFLAGS) -o $@ $^

mipsemu: main.o mips.o font.o
	$(CC) $(CFLAGS) -o $@ $^ -lSDL -lSDL_image

Makefile.dependencies:: $(SOURCES) $(HEADERS)
	$(CC) $(CFLAGS) -MM $(SOURCES) > Makefile.dependencies

-include Makefile.dependencies

.PHONY: clean

clean:
	@rm -fr $(TARGETS) $(OBJECTS) core Makefile.dependencies
