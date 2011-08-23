# Makefile for portping and others

all:

.PHONY: portping

.bin:
	mkdir -p .bin

portping: .bin
	@echo "	CCLD .bin/portping"
	@ gcc -O2 -Wall portping.c -o .bin/portping


portping-mingw32: .bin
	gcc -O2 -Wall -Werror portping.c -o .bin/portping.exe -lws2_32

clean:
	rm -r .bin

