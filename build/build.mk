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
COMMON        := config.h $(wildcard include/*.h)

LIBUVD        := build/libuv
COMMONCFLAGS  := -std=gnu99 -fPIC -Wall -Wno-unused-function -I$(LIBUVD)/include $(CFLAGS)
DEBUGCFLAGS   := -g -O0
RELEASECFLAGS := -O3 -DNDEBUG 
TESTLIBS      := libchirp.a libuv.a -lssl -lcrypto
### /Configuration

TESTSRCS=$(wildcard src/*_etest.c)
TESTEXECS=$(TESTSRCS:.c=)

NOTEST=$(filter-out $(wildcard src/*_test.c),$(wildcard src/*.c))
ifeq ($(MODE),debug)
	SRCS      = $(wildcard src/*.c)
	MYCFLAGS := $(COMMONCFLAGS) $(DEBUGCFLAGS)
else
	SRCS      = $(NOTEST)
	MYCFLAGS := $(COMMONCFLAGS) $(RELEASECFLAGS)
	MODE     := release
endif
BUILDS   := libchirp $(TESTEXECS)
ifneq ($(UNAME_S),Darwin)
	MYCFLAGS += -pthread
else
	MYCFLAGS += -I/usr/local/opt/openssl/include
	TESTLIBS += -L/usr/local/opt/openssl/lib
endif
OBJS=$(SRCS:.c=.o)

build: $(BUILDS) clean-coverage

libchirp-depends: | libuv

config.h: | build/config.defs.h
	cp build/config.defs.h config.h

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

clean-coverage: $(OBJS) $(COMMON) libchirp.a
ifeq ($(UNAME_S),Darwin)
	find . -name "*.gcda" -print0 | xargs -0 rm
else
	find . -name "*.gcda" -print0 | xargs -r -0 rm
endif

libuv.a: $(LIBUVD)/.libs/libuv.a
	cp  $(LIBUVD)/.libs/libuv.a libuv.a

libuv: libuv.a

libchirp: libchirp.a

libchirp.a: $(OBJS) $(COMMON) | libchirp-depends
	ar $(ARFLAGS) $@ $(OBJS)
ifneq ($(MODE),debug)
ifeq ($(UNAME_S),Darwin)
	strip -S $(OBJS)
else
	strip --strip-debug $(OBJS)
endif
endif

clean:
	git clean -xdf
	cd build/libuv && git clean -xdf

test-lib: | libuv
	CFLAGS="$(MYCFLAGS)" make -C $(LIBUVD) check

%.c: %.h $(COMMON)
	touch $@

%.o: %.c $(COMMON)
	$(CC) -c -o $@ $< $(MYCFLAGS) -Werror $(COVERAGE)

%_etest: %_etest.c libchirp.a
	$(CC) -o $@ $@.o $(TESTLIBS) $(MYCFLAGS) -Werror $(COVERAGE) $(LDFLAGS)
