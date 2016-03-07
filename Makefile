.PHONY: clean libc4irp sublibs libuv mbedtls test_ext

PROJECT   := c4irp
export CC := clang

COMMON    := config.h c4irpc/common.h
CCFLAGS   := -fPIC -Wall -Werror -Wno-unused-function -Iinclude
DCFLAGS   := $(CCFLAGS) -g --coverage
PCFLAGS   := $(CCFLAGS) -O3 -DNDEBUG
CFLAGS    := $(DCFLAGS)
LDFLAGS   := libc4irp.a
PYINC     := $(shell python-config --includes)
PYLD      := $(shell python-config --ldflags)

SRCS=$(wildcard c4irpc/*.c)
OBJS=$(SRCS:.c=.o)

include home/Makefile

all: libc4irp

test-all: all pymods test coverage

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

pymods: c4irp/_high_level.so

c4irp/_high_level.so: libc4irp.a cffi/high_level.py
	cd c4irp && python high_level.py && mv cffi/_high_level.ch c4irp/
	$(CC) -std=c99 -shared -o $@ c4irp/_high_level.c $(CFLAGS) $(PYINC) $(LDFLAGS)

# c4irp/_low_level.so: libc4irp.a cffi/low_level.py
# 	cd c4irp && python low_level.py && mv cffi/_low_level.ch c4irp/
# 	$(CC) -std=c99 -shared -o $@ c4irp/_low_level.c $(CFLAGS) $(PYINC) $(LDFLAGS)

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

array_test: c4irpc/array_test.o
	$(CC) -std=c99 -o $@ $< $(CFLAGS)

libc4irp: libc4irp.a

libc4irp.a: $(OBJS)
	@make CFLAGS="$(PCFLAGS)" libuv mbedtls
	ar $(ARFLAGS) $@ $^

clean:
	git clean -xdf
	cd libuv && git clean -xdf
	cd mbedtls && git clean -xdf

test_ext: array_test
	make CFLAGS="$(DCFLAGS)"
	make -C mbedtls check
	./array_test 2>&1 | grep Bufferoverflow

coverage:
	geninfo --config-file lcovrc c4irpc \
		-o c4irpc.info --derive-func-data
	lcov --config-file lcovrc \
		-a c4irpc.info \
		-o app_total.info \
		| grep -v -E \
'(Summary coverage rate|\
.: no data found|\
Combining tracefiles.|\
Reading tracefile)' \
		| grep -v "app_.*.info" \
		| grep -v '.: 100.0%' 2>&1 | ( ! grep . )
