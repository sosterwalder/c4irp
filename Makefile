.PHONY: clean libuv mbedtls test_ext

PROJECT   := c4irp
export CC := clang

COMMON    := config.h c4irp/common.h
# CFLAGS    := -O3 -DNDEBUG


include home/Makefile

config.h: config.defs.h
	cp config.defs.h config.h

test_ext: array_test
	./array_test 2>&1 | grep Bufferoverflow

libuv/configure:
	cd libuv && ./autogen.sh

libuv/Makefile: libuv/configure
	cd libuv && ./configure

libuv/.libs/libuv.a: libuv/Makefile
	make -C libuv

libuv: libuv/.libs/libuv.a

mbedtls/library/libmbedtls.a:
	make -C mbedtls

mbedtls: mbedtls/library/libmbedtls.a

%.c: %.h

%.o: %.c $(COMMON)
	$(CC) -std=c99 -c -o $@ $< $(CFLAGS)

array_test: c4irp/array_test.o
	$(CC) -std=c99 -o $@ $< $(CFLAGS)

clean:
	git clean -xdf
	cd libuv && git clean -xdf
	cd mbedtls && git clean -xdf

