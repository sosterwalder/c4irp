.PHONY: clean libchirp sublibs libuv test_ext doc c4irp test_dep

PROJECT     := c4irp

PYPY      := $(shell python --version 2>&1 | grep PyPy > /dev/null 2> /dev/null; echo $$?)
NEWCOV    := $(shell llvm-cov --help 2>&1 | grep -E "USAGE:.*SOURCEFILE" > /dev/null 2> /dev/null; echo $$?)

ifeq ($(PYPY),0)
	COVERAGE  :=
else
	COVERAGE  := --coverage
endif

COMMON  := config.h src/common.h
CCFLAGS := -Wall -Werror -Wno-unused-function -Ilibuv/include
MYFLAGS := -std=gnu99

ifneq ($(OS),Windows_NT)
	CCFLAGS += -fPIC
	UNAME_S := $(shell uname -s)
	ifneq ($(UNAME_S),Darwin)
		MYFLAGS += -pthread
	endif
endif

DCFLAGS := $(CCFLAGS) -g $(COVERAGE)
PCFLAGS := $(CCFLAGS) -O3 -DNDEBUG
CFFIF   := $(shell pwd)/pyproject/cffi_fix:$(PATH)
PY      := python
MYCC    := clang

SETCFLAGS := $(DCFLAGS) $(MYFLAGS)

DOCC=$(wildcard src/*.c)
DOCH=$(wildcard src/*.h) $(wildcard include/*.h)
DOCRST=$(DOCC:.c=.c.rst) $(DOCH:.h=.h.rst)
SRCS=$(wildcard src/*.c)
TESTSRCS=$(wildcard src/*_etest.c)
OBJS=$(SRCS:.c=.o)
TESTEXECS=$(TESTSRCS:.c=)
COVOUT=$(SRCS:.c=.c.gcov)

include pyproject/Makefile

FAIL_UNDER := 95  # TODO: remove!!

all: pre-install pymods test-execs  ## Build for development (make setup.py or make.release for production)

vi:  ## Start a vim editing the imporant files
	vim TODO.rst src/*.c src/*.h c4irp/*.py cffi/*.py include/*.h doc/ref/* config.defs.h

lldb: all  ## Build and run py.test in lldb
	echo lldb `pyenv which python` -- -m pytest -x
	lldb `pyenv which python` -- -m pytest -x

doc-all: all $(DOCRST) doc  ## Build using c2rst and then generate docs

test-all: all cpp-check test coverage test-lib  ## Build and test everything

test-nolib: all cpp-check test coverage  ## Build and test without external libs

cpp-check:  ## Run cppcheck on the project
	cppcheck -v --std=c99 -Iinclude -Ilibuv/include --config-exclude=libuv/include -D_SGLIB__h_ --error-exitcode=1 --inline-suppr --enable=warning,style,performance,portability,information,missingInclude src/

test_dep: all

test_ext: coala

pre-install: libchirp.a install-edit

test-cov: clean all pytest coverage  ## Clean, build and test (so coverage is correct)

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:  ## Generate html coverage report
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

pymods: _chirp_cffi.o _chirp_low_level.o  ## Build the python modules

_chirp_cffi.o: libchirp.a cffi/high_level.py
	CC="$(MYCC)" CFLAGS="$(SETCFLAGS)" PATH="$(CFFIF)" \
	   $(PY) cffi/high_level.py
	rm _chirp_cffi.c

_chirp_low_level.o: libchirp.a cffi/low_level.py
	CC="$(MYCC)" CFLAGS="$(SETCFLAGS)" PATH="$(CFFIF)" \
	   $(PY) cffi/low_level.py
	rm _chirp_low_level.c

libuv/configure:
ifeq ($(OS),Windows_NT)
	echo Not needed on windows
else
	cd libuv && ./autogen.sh
endif

libuv/Makefile: libuv/configure
ifeq ($(OS),Windows_NT)
	echo Not needed on windows
else
	cd libuv && ./configure
endif

libuv/.libs/libuv.a: libuv/Makefile
ifeq ($(OS),Windows_NT)
	make -f Makefile.mingw -C libuv
else
	make -C libuv
endif


libuv: libuv/.libs/libuv.a

%.c.rst: %.c
	pyproject/c2rst $<

%.h.rst: %.h
	pyproject/c2rst $<

%.c: %.h

%.o: %.c $(COMMON) $(DOCH)
	$(MYCC) -c -o $@ $< $(SETCFLAGS)

%_etest: %_etest.c libchirp.a | libuv
	$(MYCC) -o $@ $@.o libchirp.a libuv.a $(SETCFLAGS)

test-execs: $(TESTEXECS)

libchirp: libchirp.a

libchirp.a: $(OBJS) | libchirp-depends
	ar $(ARFLAGS) $@ $^

libchirp-depends:
	@make CFLAGS="$(CCFLAGS) -g" libuv

clean:  ## Clean only chirp not submodules
	git clean -xdf

dist-clean:
	echo "Cleaning using git"; \
	git clean -xdf -e .vagrant -e FINJA; \
	git submodule foreach --recursive 'git clean -xdf -e .vagrant -e FINJA'; \

test-lib: | libuv  ## Test dependency libs
ifeq ($(OS),Windows_NT)
	echo Ignored on windows
else
	make -C libuv CFLAGS="$(PCFLAGS)" check
endif

ifeq ($(PYPY),0)
coverage:
else
coverage: $(COVOUT)
endif


ifeq ($(NEWCOV),0)
%.c.gcov: %.c
	llvm-cov $<
	mv *.c.gcov src/
	!(grep -v "// NOCOV" $@ | grep -E "\s+#####:")
else
%.c.gcov: %.c
	echo not suppored
endif
