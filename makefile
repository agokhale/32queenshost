APPS = mcd
CC = clang
OBJECTS = colortx.o bridge.o i2.o pca9635.o pca9548.o 
#LINKFLAGS += ${LIBBASE}/mutilities/libmutil.o   ${OBJECTS} -pg
LINKFLAGS += ${LIBBASE}/mutilities/libmutil.o   ${OBJECTS}  -lm -ltiff 
CFLAGS += -g

.include "${HOME}/src/lib/ash.mk"

talk:
	cu -l ${serialdev} -s 230400
serpipe = serial | hd


core:
	genbytes.py  0x01 | serial | hd
mode0:
	genbytes.py 0x05 0xfe 0x80 0x00 0x00 | serial | hd
mode1: 
	genbytes.py 0x07 0xfe 0x94 0xaa 0xaa 0xaa 0xaa | serial | hd

blinkymode2:
	genbytes.py 0x4 0xfe 0x01 0x08 | ${serpipe}
fullpower:
	genbytes.py 0x0f 0xfe 0x82 0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xff 0xff | serial | hd
full2:
	genbytes.py 0x07 0xfe 0x8c 0xff 0xff 0xff 0xff | serial | hd

letitbe:
	genbytes.py 0x0f 0xfe 0x82 0x11 0x20 0x30 0x40 0x50 0x60 0x70 0x80 0x90 0xa0 0xb0 0xc0 | serial | hd

letitbeoff:
	genbytes.py 0x0f 0xfe 0x82 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 | serial | hd
	genbytes.py 0x0f 0xfe 0x86 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 | serial | hd
read:
	
	genbytes.py 0x1f 0xfe 0x80 0x04 0x05 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf | serial | hd
	genbytes.py 0x1f 0xfe 0x8b 0x04 0x05 0x6 0x7 0x8 0x9 0xa 0xb 0xc 0xd 0xe 0xf | serial | hd
resetbridge:
	genbytes.py 0x01 | serial | hd 

val:
	valgrind --leak-check=full  --track-origins=yes --show-reachable=yes mcd
scan-build:
	make clean
	scan-build -v make
