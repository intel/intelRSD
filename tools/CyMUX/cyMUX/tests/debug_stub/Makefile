#   Copyright (c) 2015
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the debug_stub for the test cases
#
#   Originating Author: Brian Kurle
#   Creation Date:      18 Dec 2015
#
#   History:
#   18-Dec-2015 bjk - Initial creation

# Standard object file dependency
%.o : %.c
	$(CC) $(CFLAGS) $(ALTFLAGS) -c $< -o $*.o

# Pseudo deendency management of changed include files
.dep/%.ho : %.h
	@if [ ! -d .dep ]; then mkdir -p .dep; fi
	@touch $@


CC=gcc
CFLAGS  = -std=c99 -pedantic-errors -Wall -W -Werror -Wformat=2 -Wmissing-declarations -Wmissing-prototypes -Winline -Wno-char-subscripts -Wwrite-strings -Wundef -Wnested-externs -Wmissing-braces -Wparentheses -Wreturn-type -Wswitch -Wimplicit-int -Wimplicit-function-declaration -Wunreachable-code -Wunused-variable -Wunused-value -Wuninitialized -Wsign-compare
#CFLAGS  = -Wstrict-prototypes -Winline
CFLAGS += -I.. -I../..
CFLAGS += -g
CFLAGS += -D_GNU_SOURCE

LDFLAGS  = -ltap -L ../tap

TARGETS  = debug_stub.o


all: .dep/Makefile $(TARGETS)

release:

.dep/Makefile: Makefile
	make clean
	@if [ ! -d .dep ]; then mkdir -p .dep; fi
	@touch $@

install:

uninstall:

clean:
	rm -rf $(TARGETS) *\.o .dep

over: clean all

help:
	@echo	'make		would compile and create the library and create a link'
	@echo	'make clean	would remove the library and the soft link to the library (soname)'

lines:
	wc -l *\.c *\.h

tests: all check

check:

refresh:
	../../dep.pl --include ../..

# Target binaries to build

### Generated targets ###
# Generated via ../../dep.pl --include ../..
# Object file nested dependencies
debug_stub.o :: debug_stub.c ../../.dep/debug.ho

# Include file nested dependencies

../../.dep/debug.ho :: ../../debug.h ../../.dep/IPMB.ho
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/IPMB.ho :: ../../IPMB.h
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@
