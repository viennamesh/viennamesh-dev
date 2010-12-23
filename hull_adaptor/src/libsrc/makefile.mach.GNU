#
# System dependent make include file
# GNU compiler for any hardware
#
CC=g++
CPLUSPLUS=$(CC)
AR=ar
LINK=$(CC)
MAKE=make
RM=rm
RANLIB=ranlib
#
# Machine dependent flags:
#
CFLAGS2 =
CPLUSPLUSFLAGS2 = -O2 -I/usr/local/gcc/lib -I/usr/include/X11R5
LINKFLAGS2 = -L/usr/lib/X11R5
