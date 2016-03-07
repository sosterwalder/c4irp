PROJECT   := c4irp
export CC := clang

include home/Makefile


clean:
	rm -f libuv/configure
	rm -f libuv/Makefile
	rm -f libuv/.libs/libuv.a
	cd libuv && git clean -xdf

libuv/configure:
	cd libuv && ./autogen.sh

libuv/Makefile: libuv/configure
	cd libuv && ./configure

libuv/.libs/libuv.a: libuv/Makefile
	make -C libuv

libuv: libuv/.libs/libuv.a
