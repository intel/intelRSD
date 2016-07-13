#   Copyright (c) 2015
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the IPMB test cases
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

TARGETS  = 01-valid_request-t 02-valid_msg-t 03-craft_error_response-t


all: .dep/Makefile $(TARGETS)

release:

.dep/Makefile: Makefile
	make clean
	@if [ ! -d .dep ]; then mkdir -p .dep; fi
	@touch $@

install: all

uninstall:

clean:
	rm -rf $(TARGETS) *\.o .dep

over: clean all

help:
	@echo	'make		would compile and create the library and create a link'
	@echo	'make clean	would remove the library and the soft link to the library (soname)'

lines:
	wc -l *\.c *\.h

tests: check

check: TESTS
	runtests -l TESTS

TESTS: .dep/Makefile $(TARGETS)
	ls -1 *-t >TESTS

refresh:
	../../dep.pl --include ../..

# Target binaries to build
01-valid_request-t: valid_request.o ../debug_stub/debug_stub.o ../../IPMB.o ../../sys_send.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

02-valid_msg-t: valid_msg.o ../debug_stub/debug_stub.o ../../IPMB.o ../../sys_send.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

03-craft_error_response-t: craft_error_response.o ../debug_stub/debug_stub.o ../../IPMB.o ../../sys_send.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

### Generated targets ###
# Generated via ../../dep.pl --include ../..
# Object file nested dependencies
craft_error_response.o :: craft_error_response.c ../../.dep/IPMB.ho ../../.dep/shim.ho ../../.dep/debug.ho
valid_msg.o :: valid_msg.c ../../.dep/IPMB.ho
valid_request.o :: valid_request.c ../../.dep/IPMB.ho

# Include file nested dependencies

../../.dep/debug.ho :: ../../debug.h ../../.dep/IPMB.ho
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/IPMB.ho :: ../../IPMB.h
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/shim.ho :: ../../shim.h
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@
