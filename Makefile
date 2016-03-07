.PHONY: clean libc4irp sublibs libuv mbedtls test_ext

PROJECT     := c4irp
export CC   := clang

PYPY      := $(shell python --version 2>&1 | grep PyPy > /dev/null 2> /dev/null; echo $$?)
NEWCOV    := $(shell llvm-cov --help 2>&1 | grep -E "USAGE:.*SOURCEFILE" > /dev/null 2> /dev/null; echo $$?)

ifeq ($(PYPY),0)
	COVERAGE  :=
else
	COVERAGE  := --coverage
endif

COMMON    := config.h c4irpc/common.h
CCFLAGS   := -fPIC -Wall -Werror -Wno-unused-function -Iinclude
DCFLAGS   := $(CCFLAGS) -g $(COVERAGE)
PCFLAGS   := $(CCFLAGS) -O3 -DNDEBUG
CFFIF     := $(shell pwd)/home/cffi_fix:$(PATH)
PY        := python

export CFLAGS   := $(DCFLAGS)

SRCS=$(wildcard c4irpc/*.c)
OBJS=$(SRCS:.c=.o)
COVOUT=$(SRCS:.c=.c.gcov)

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
	PATH=$(CFFIF) $(PY) cffi/high_level.py
	mv _high_level* c4irp/

# c4irp/_low_level.so: libc4irp.a cffi/low_level.py
#	PATH=$(CFFIF) CFFILIBS=libc4irp.a $(PY) cffi/low_level.py
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


ifeq ($(PYPY),0)
coverage:
else
coverage: $(COVOUT)
endif


ifeq ($(NEWCOV),0)
%.c.gcov: %.c
	llvm-cov $< | grep "Lines executed:100.00%"
	mv *.c.gcov c4irpc/
else
%.c.gcov: %.c
	llvm-cov-3.4 -gcda=$(basename $<).gcda -gcno=$(basename $<).gcno
	mv *.c.gcov c4irpc/
endif
