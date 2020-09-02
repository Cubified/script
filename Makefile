all: script

CC=gcc
TCC=tcc

LIBS=

FLAGS=-Os -s -pipe -DIS_DEBUG_BUILD
DEBUGFLAGS=-Og -Wall -pipe -g -DIS_DEBUG_BUILD
RELEASEFLAGS=-O3 -s -pipe

SOURCES=script.c
OUT=script

RM=/bin/rm
CP=/bin/cp

.PHONY: script
script:
	$(CC) $(SOURCES) -o $(OUT) $(LIBS) $(FLAGS)

debug:
	$(CC) $(SOURCES) -o $(OUT) $(LIBS) $(DEBUGFLAGS)

release:
	$(CC) $(SOURCES) -o $(OUT) $(LIBS) $(RELEASEFLAGS)

clean:
	if [ -e "$(OUT)" ]; then $(RM) $(OUT); fi
