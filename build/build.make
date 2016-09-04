.PHONY: clean

all: build
PYPY      := $(shell python --version 2>&1 | grep PyPy > /dev/null 2> /dev/null; echo $$?)
UNAME_S   := $(shell uname -s)
ifeq ($(PYPY),0)
	COVERAGE  :=
else
	COVERAGE  := --coverage
endif

### Configuration
COMMON        := config.h include/common.h

LIBUVD        := build/libuv
COMMONCFLAGS  := -std=gnu99 -fPIC -Wall -Werror -Wno-unused-function -I$(LIBUVD)/include -fPIC
DEBUGCFLAGS   := -g $(COVERAGE)
RELEASECFLAGS := -O3 -DNDEBUG 
TESTLIBS      := libchirp.a libuv.a
### /Configuration

DOCC=$(wildcard src/*.c)
DOCH=$(wildcard src/*.h) $(wildcard include/*.h)
DOCRST=$(DOCC:.c=.c.rst) $(DOCH:.h=.h.rst)
TESTSRCS=$(wildcard src/*_etest.c)
TESTEXECS=$(TESTSRCS:.c=)

NOTEST=$(filter-out $(wildcard src/*_test.c),$(wildcard src/*.c))
ifeq ($(MODE),debug)
	SRCS      = $(wildcard src/*.c)
	MYCFLAGS := $(COMMONCFLAGS) $(DEBUGCFLAGS)
	BUILDS   := libchirp $(TESTEXECS)
else
	SRCS      =$(filter-out $(wildcard src/*_etest.c),$(NOTEST))
	MYCFLAGS := $(COMMONCFLAGS) $(RELEASECFLAGS)
	MODE     := release
	BUILDS   := libchirp
endif
ifneq ($(UNAME_S),Darwin)
	MYCFLAGS += -pthread
endif
OBJS=$(SRCS:.c=.o)

build: $(BUILDS)

libchirp-depends: | libuv

config.h: | config.defs.h
	cp config.defs.h config.h

$(LIBUVD)/configure:
	cd $(LIBUVD) && ./autogen.sh

$(LIBUVD)/Makefile: $(LIBUVD)/configure
	cd $(LIBUVD) && CFLAGS="$(MYCFLAGS)" ./configure

$(LIBUVD)/.libs/libuv.a: $(LIBUVD)/Makefile
	CFLAGS="$(MYCFLAGS)" make -C $(LIBUVD)
ifneq ($(MODE),debug)
ifeq ($(UNAME_S),Darwin)
	strip -S $@
else
	strip --strip-debug $@
endif
endif

libuv.a: $(LIBUVD)/.libs/libuv.a
	cp  $(LIBUVD)/.libs/libuv.a libuv.a

libuv: libuv.a

libchirp: libchirp.a

libchirp.a: $(OBJS) | libchirp-depends
	ar $(ARFLAGS) $@ $^
ifneq ($(MODE),debug)
ifeq ($(UNAME_S),Darwin)
	strip -S $^
else
	strip --strip-debug $^
endif
endif

clean:
	git clean -xdf
	cd build/libuv && git clean -xdf

%.c: %.h

%.o: %.c $(COMMON)
	$(CC) -c -o $@ $< $(MYCFLAGS)

%_etest: %_etest.c libchirp
	$(CC) -o $@ $@.o $(TESTLIBS) $(MYCFLAGS) $(LDFLAGS)