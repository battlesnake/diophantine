CC ?= gcc
O ?= fast
CFLAGS = -Wall -Wextra -pedantic -O$(O) -flto -fopenmp -lm -ffunction-sections -fdata-sections -Wl,--gc-sections -march=native -mtune=native -masm=intel -Wno-unused-function -g

RMF = rm -f --

.PHONY: all
all: main

.PHONY: run
run: all
	./main $(test)

.PHONY: clean
clean:
	$(RMF) main

main: main.c binary_search.c modulo.c estimate_root.c
	$(CC) $(CFLAGS) -o $@ $^
