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
CINCS      = -I../../bctool -I../../firmware -I/usr/include
CFLAGS     = ${CINCS} -g -DWIN32=32 -std=gnu99
LDLIBS     = -L/usr/lib -L/usr/local/lib -largtable2 -lm
LDFLAGS    = -mwindows

TARGETS = rtbt

CPPCHECK_OPT=--force --inline-suppr -v --enable=all

all: clean $(TARGETS)

cppcheck:
	cppcheck ${CINCS} ${CPPCHECK_OPT} *.c

rtbt: rtbt.o
	  $(CC) $(LDFLAGS) $(CFLAGS) $(TARGETS).o -o $@ $(LDLIBS)

clean:
	rm -f *.o *~ $(TARGETS)

