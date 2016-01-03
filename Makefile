CFLAGS = -std=c11 -g -Wall -Werror
CFLAGS += -O2 -DNDEBUG -march=native -mtune=native -fomit-frame-pointer -s

AMALG = faconde.h faconde.c

#--------------------------------------
# Abstract targets
#--------------------------------------

all: $(AMALG) example test/perf

clean:
	rm -f lua/faconde.so example test/perf

check: lua/faconde.so
	cd test && ./run.sh

.PHONY: all clean check


#--------------------------------------
# Concrete targets
#--------------------------------------

faconde.h: src/api.h
	cp $< $@

faconde.c: $(wildcard src/*.c) $(wildcard src/*.h)
	src/mkamalg.py src/*.c > $@

lua/faconde.so: lua/faconde.c $(AMALG)
	$(MAKE) -C lua

example: example.c $(AMALG)
	$(CC) $(CFLAGS) $< faconde.c -o $@

test/perf: test/perf.c $(AMALG)
	$(CC) $(CFLAGS) $< faconde.c -o $@
