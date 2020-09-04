all: script

CC=gcc
TCC=tcc

LIBS=

FLAGS=-O3 -s -pipe
DEBUGFLAGS=-Og -Wall -pipe -g -DIS_DEBUG_BUILD

SOURCES=script.c
OUT=script

RM=/bin/rm
CP=/bin/cp

.PHONY: script
script:
	$(CC) $(SOURCES) -o $(OUT) $(LIBS) $(FLAGS)

debug:
	$(CC) $(SOURCES) -o $(OUT) $(LIBS) $(DEBUGFLAGS)

clean:
	if [ -e "$(OUT)" ]; then $(RM) $(OUT); fi
