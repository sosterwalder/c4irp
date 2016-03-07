.PHONY: clean libuv mbedtls test_ext

PROJECT   := c4irp
export CC := clang

COMMON    := config.h c4irp/common.h libuv mbedtls
DCFLAGS   := -Wall -Werror -Wno-unused-function -g --coverage
CFLAGS    := $(DCFLAGS)
#CFLAGS    := -Wall -Werror -Wno-unused-function -O3 -DNDEBUG

SRCS=$(wildcard c4irp/*.c)
OBJS=$(SRCS:.c=.o)

include home/Makefile

all: array_test

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

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

libchirp.a: $(OBJS)
	ar $(ARFLAGS) $@ $^

clean:
	git clean -xdf
	cd libuv && git clean -xdf
	cd mbedtls && git clean -xdf

test_ext:
	make CFLAGS="$(DCFLAGS)"
	./array_test 2>&1 | grep Bufferoverflow
	geninfo --config-file lcovrc c4irp \
		-o c4irp.info --derive-func-data
	lcov --config-file lcovrc \
		-a c4irp.info \
		-o app_total.info \
		| grep -v -E \
'(Summary coverage rate|\
.: no data found|\
Combining tracefiles.|\
Reading tracefile)' \
		| grep -v "app_.*.info" \
		| grep -v '.: 100.0%' 2>&1 | ( ! grep . )
