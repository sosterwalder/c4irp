.PHONY: clean libc4irp sublibs libuv test_ext doc c4irp test_dep

PROJECT     := c4irp

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
MYCC      := clang

SETCFLAGS := $(DCFLAGS) $(MYFLAGS)

DOCC=$(wildcard c4irpc/*.c)
DOCH=$(wildcard c4irpc/*.h) $(wildcard include/*.h)
DOCRST=$(DOCC:.c=.c.rst) $(DOCH:.h=.h.rst)
SRCS=$(wildcard c4irpc/*.c)
OBJS=$(SRCS:.c=.o)
COVOUT=$(SRCS:.c=.c.gcov)

include pyproject/Makefile

all: pre-install pymods

vi:
	vim c4irpc/*.c c4irpc/*.h c4irp/*.py cffi/*.py include/*.h doc/ref/*

lldb: all
	lldb `pyenv which python` -- -m pytest -x

doc-all: all $(DOCRST) doc

test-all: all test test-array coverage test-lib

test_dep: all

pre-install: libc4irp.a install-edit

test-cov: clean all pytest test-array coverage

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

pymods: _c4irp_cffi.o _c4irp_low_level.o

_c4irp_cffi.o: libc4irp.a cffi/high_level.py
	CC="$(MYCC)" CFLAGS="$(SETCFLAGS)" PATH="$(CFFIF)" \
	   $(PY) cffi/high_level.py
	rm _c4irp_cffi.c

_c4irp_low_level.o: libc4irp.a cffi/low_level.py
	CC="$(MYCC)" CFLAGS="$(SETCFLAGS)" PATH="$(CFFIF)" \
	   $(PY) cffi/low_level.py
	rm _c4irp_low_level.c

libuv/configure:
	cd libuv && ./autogen.sh

libuv/Makefile: libuv/configure
	cd libuv && ./configure

libuv/.libs/libuv.a: libuv/Makefile
	make -C libuv

libuv: libuv/.libs/libuv.a

%.c.rst: %.c
	pyproject/c2rst $<

%.h.rst: %.h
	pyproject/c2rst $<

%.c: %.h

%.o: %.c $(COMMON) $(DOCH)
	$(MYCC) -c -o $@ $< $(SETCFLAGS)

array_test: c4irpc/array_test.o
	$(MYCC) -o $@ $< $(SETCFLAGS)

libc4irp: libc4irp.a

libc4irp.a: $(OBJS) | libc4irp-depends
	ar $(ARFLAGS) $@ $^

libc4irp-depends:
	@make CFLAGS="$(CCFLAGS) -g" libuv

clean-all: clean clean-sub

clean:
	git clean -xdf

clean-sub:
	cd libuv && git clean -xdf

test-array: array_test
	./array_test 1 2>&1
	./array_test 3 2>&1 | grep Bufferoverflow
	./array_test -1 2>&1 | grep Bufferoverflow

test-lib:
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
