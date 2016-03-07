.PHONY: clean libc4irp sublibs libuv mbedtls test_ext

PROJECT   := c4irp
export CC := clang

COMMON    := config.h c4irpc/common.h
CCFLAGS   := -fPIC -Wall -Werror -Wno-unused-function 
DCFLAGS   := $(CCFLAGS)-g --coverage
PCFLAGS   := $(CCFLAGS) -O3 -DNDEBUG
CFLAGS    := $(DCFLAGS)

SRCS=$(wildcard c4irpc/*.c)
OBJS=$(SRCS:.c=.o)
HL=$(wildcard c4irp/_high_level*.so)
# LL=$(wildcard c4irp/_low_level*.so)

ifeq ($(HL),)
	HL=undefhl
endif

# ifeq ($(LL),)
# 	LL=undefll
# endif

include home/Makefile

all: libc4irp $(HL) $(LL)

sublibs:
	make CFLAGS="$(PCFLAGS)" libuv mbedtls

config.h: config.defs.h
	cp config.defs.h config.h

genhtml:
	mkdir -p lcov_tmp
	cd lcov_tmp && genhtml --config-file ../lcovrc ../app_total.info
	cd lcov_tmp && open index.html

$(HL): libc4irp c4irp/high_level.py
	cd c4irp && python high_level.py

# $(LL): libc4irp c4irp/low_level.py
#	cd c4irp && python low_level.py

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

libc4irp: sublibs libc4irp.a

libc4irp.a: $(OBJS)
	ar $(ARFLAGS) $@ $^

clean:
	git clean -xdf
	cd libuv && git clean -xdf
	cd mbedtls && git clean -xdf

test_ext: array_test
	make CFLAGS="$(DCFLAGS)"
	make -C mbedtls check
	./array_test 2>&1 | grep Bufferoverflow
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
