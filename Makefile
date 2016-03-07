.PHONY: clean libc4irp sublibs libuv mbedtls test_ext

PROJECT     := c4irp
export CC   := clang

COMMON    := config.h c4irpc/common.h
CCFLAGS   := -fPIC -Wall -Werror -Wno-unused-function -Iinclude
DCFLAGS   := $(CCFLAGS) -g --coverage
PCFLAGS   := $(CCFLAGS) -O3 -DNDEBUG
CFFIF     := $(shell pwd)/home/cffi_fix:$(PATH)
PY        := python

export CFLAGS   := $(DCFLAGS)
export CFFILIBS := libc4irp.a

SRCS=$(wildcard c4irpc/*.c)
OBJS=$(SRCS:.c=.o)
COVOUT=$(SRCS:.c=.gcov)

include home/Makefile

all: libc4irp

test-all: all pymods test coverage

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

pymods: c4irp/_high_level.o

c4irp/_high_level.o: libc4irp.a cffi/high_level.py
	PATH=$(CFFIF) $(PY) cffi/high_level.py --linker_exe=$(CC)
	mv _high_level* c4irp/

# c4irp/_low_level.so: libc4irp.a cffi/low_level.py
#	PATH=$(CFFIF) $(PY) cffi/low_level.py --linker_exe=$(CC)
#	mv _low_level* c4irp/

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
	make -C mbedtls check
	make -C libuv CFLAGS="$(PCFLAGS)" check
	./array_test 1 2>&1
	./array_test 3 2>&1 | grep Bufferoverflow
	make coverage

coverage: $(COVOUT)

%.gcov: %.c
	llvm-cov -n $< | grep "Lines executed:100.00%"
