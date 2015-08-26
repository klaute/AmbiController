#######################################################################
#
# (C) 2014 by Kai Lauterbach
#
# License: GPLv3
#
#######################################################################
#
#SHELL  = /bin/sh

#the default make rule for compiling C source code is
# xxx.o xxx.c:
#   $(CC) -c $(CFLAGS) -o xxx.o xxx.c

CC         = gcc
#CC         = i686-pc-cygwin-gcc-4.8.2
#CC = i686-w64-mingw32-gcc
CINCS      = -I../../firmware/ -I../firmware/ -I/usr/local/include
CFLAGS     = `sdl2-config --cflags` ${CINCS}
CFLAGS    += -g -DWIN32=32 -std=gnu99
#LDLIBS     = `sdl2-config --libs` -pthread -lm
LDLIBS     = `sdl2-config --libs` -lm
LDLIBS    += -L/usr/lib -L/usr/local/lib -largtable2 -lm
LDFLAGS    = -mwindows

TARGETS = bctool

CPPCHECK_OPT=--force --inline-suppr -v --enable=all

all: clean runscript chmod $(TARGETS)

cppcheck:
	cppcheck ${CINCS} ${CPPCHECK_OPT} *.c

runscript:
	sh gen_code.sh

bctool: bctool.o
	  $(CC) $(LDFLAGS) $(TARGETS).o -o $@ $(LDLIBS)

clean:
	rm -f *.o *~ $(TARGETS)

chmod:
	chmod a+x *.dll

