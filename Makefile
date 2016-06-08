.PHONY: clean libc4irp sublibs libuv mbedtls test_ext doc c4irp

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
CCFLAGS   := -fPIC -Wall -Werror -Wno-unused-function -Ilibuv/include
MYFLAGS   := -std=gnu99 -pthread
DCFLAGS   := $(CCFLAGS) -g $(COVERAGE)
PCFLAGS   := $(CCFLAGS) -O3 -DNDEBUG
CFFIF     := $(shell pwd)/pyproject/cffi_fix:$(PATH)
PY        := python

export CFLAGS   := $(DCFLAGS) $(MYFLAGS)

DOCC=$(wildcard c4irpc/*.c)
DOCH=$(wildcard c4irpc/*.h) $(wildcard include/*.h)
DOCRST=$(DOCC:.c=.c.rst) $(DOCH:.h=.h.rst)
SRCS=$(wildcard c4irpc/*.c)
OBJS=$(SRCS:.c=.o)
COVOUT=$(SRCS:.c=.c.gcov)

include pyproject/Makefile

all: pre-install pymods

vi:
	vi c4irpc/*.c c4irpc/*.h c4irp/*.py cffi/*.py include/*.h

lldb:
	lldb `pyenv which python` -- -m pytest -x

doc-all: $(DOCRST) doc

test-all: pre-install pymods test test-array coverage test-lib

pre-install: libc4irp.a
	CC="clang -Qunused-arguments" pip install --upgrade -r .requirements.txt -e .

test-cov: clean pre-install pymods pytest test-array coverage

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

pymods: _c4irp_cffi.o

_c4irp_cffi.o: libc4irp.a cffi/high_level.py
	PATH=$(CFFIF) $(PY) cffi/high_level.py
	rm _c4irp_cffi.c

# _c4irp_low_level.o: libc4irp.a cffi/low_level.py
#	PATH=$(CFFIF) $(PY) cffi/low_level.py
#	rm _c4irp_low_level.c

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

%.c.rst: %.c
	pyproject/c2rst $<

%.h.rst: %.h
	pyproject/c2rst $<

%.c: %.h

%.o: %.c $(COMMON) $(DOCH)
	$(CC) -c -o $@ $< $(CFLAGS)

array_test: c4irpc/array_test.o
	$(CC) -o $@ $< $(CFLAGS)

libc4irp: libc4irp.a

libc4irp.a: $(OBJS) | libc4irp-depends
	ar $(ARFLAGS) $@ $^

libc4irp-depends:
	@make CFLAGS="$(PCFLAGS)" libuv mbedtls

clean-all: clean clean-sub

clean:
	git clean -xdf

clean-sub:
	cd libuv && git clean -xdf
	cd mbedtls && git clean -xdf

test-array: array_test
	./array_test 1 2>&1
	./array_test 3 2>&1 | grep Bufferoverflow
	./array_test -1 2>&1 | grep Bufferoverflow

test-lib:
	make -C mbedtls check
	make -C libuv CFLAGS="$(PCFLAGS)" check


ifeq ($(PYPY),0)
coverage:
else
coverage: $(COVOUT)
endif


ifeq ($(NEWCOV),0)
%.c.gcov: %.c
	llvm-cov $<
	mv *.c.gcov c4irpc/
	!(grep -v "// NOCOV" $@ | grep -E "\s+#####:")
else
%.c.gcov: %.c
	echo not suppored
endif
