PREFIX = /usr/local
LUA_VERSION = 5.2

CFLAGS = -std=c11 -fPIC -shared -g -Wall -Werror
CFLAGS += -O2 -DNDEBUG -march=native -mtune=native

LIB = faconde.so

all: $(LIB)

clean:
	rm -f $(LIB)

install: $(LIB)
	install -pm 0755 $(LIB) $(PREFIX)/lib/lua/$(LUA_VERSION)/$(LIB)

uninstall:
	rm -f $(PREFIX)/lib/lua/$(LUA_VERSION)/$(LIB)

.PHONY: all clean install uninstall

$(LIB): faconde.c ../faconde.c
	$(CC) $(CFLAGS) -o $@ $^
