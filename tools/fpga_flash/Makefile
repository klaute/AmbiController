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

CC       = gcc -O3
CINCS    = -I../../bctool/ -I../../firmware/ -I/usr/local/include
CFLAGS   = ${CINCS} -g -DWIN32=32
LDLIBS   = -L/usr/lib -L/usr/local/lib -largtable2
LDFLAGS  = -mwindows

TARGETS = fpga_flash

CPPCHECK_OPT=--force --inline-suppr -v --enable=all

all: clean $(TARGETS)

cppcheck:
	cppcheck ${CINCS} ${CPPCHECK_OPT} *.c

fpga_flash: fpga_flash.o
	  $(CC) $(CFLAGS) $(LDFLAGS) fpga_flash.o -o $@ $(LDLIBS)

clean:
	rm -f *.o *~ $(TARGETS)

