#   Copyright (c) 2015
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the ll (linked list) test cases
#
#   Originating Author: Brian Kurle
#   Creation Date:      21 Dec 2015
#
#   History:
#   21-Dec-2015 bjk - Initial creation

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
CFLAGS += -D_TAP_SOURCE

LDFLAGS  = -ltap -L ../tap

TARGETS  = 01-init-t 02-destroy-t 03-add-t 04-post-t 05-find-t 06-get-t 07-flush-t


all: .dep/Makefile $(TARGETS)

release:

.dep/Makefile: Makefile
	@make clean
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

TESTS: all
	ls -1 *-t >TESTS

refresh:
	../../dep.pl --include ../.. --include ..

# Target binaries to build
01-init-t: init.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

02-destroy-t: destroy.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

03-add-t: add.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

04-post-t: post.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

05-find-t: find.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

06-get-t: get.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

07-flush-t: flush.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

### Generated targets ###
# Generated via ../../dep.pl --include ../.. --include ..
# Object file nested dependencies
add.o :: add.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
destroy.o :: destroy.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
find.o :: find.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
flush.o :: flush.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
get.o :: get.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
init.o :: init.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
post.o :: post.c ../../ll.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
../../ll.c :: ../../.dep/ll.ho ../../.dep/IPMB.ho ../../.dep/debug.ho ../../.dep/shim.ho

# Include file nested dependencies

../../.dep/shim.ho :: ../../shim.h
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../tap/.dep/basic.ho :: ../tap/basic.h
	@if [ ! -d ../tap/.dep/ ]; then mkdir -p ../tap/.dep/; fi
	@touch $@

../../.dep/ll.ho :: ../../ll.h ../../.dep/IPMB.ho
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/IPMB.ho :: ../../IPMB.h
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/debug.ho :: ../../debug.h ../../.dep/IPMB.ho
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../mock_pthread/.dep/mock_pthread.ho :: ../mock_pthread/mock_pthread.h
	@if [ ! -d ../mock_pthread/.dep/ ]; then mkdir -p ../mock_pthread/.dep/; fi
	@touch $@
