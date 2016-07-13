#   Copyright (c) 2015
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the msgq test cases
#
#   Originating Author: Brian Kurle
#   Creation Date:      30 Dec 2015
#
#   History:
#   30-Dec-2015 bjk - Initial creation

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

TARGETS  = 01-init-t 02-destroy-t 03-post-t 04-get-t


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
01-init-t: msgq_init.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

02-destroy-t: msgq_destroy.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

03-post-t: msgq_post.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

04-get-t: msgq_get.o ../debug_stub/debug_stub.o ../mock_pthread/mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

### Generated targets ###
# Generated via ../../dep.pl --include ../.. --include ..
# Object file nested dependencies
msgq_destroy.o :: msgq_destroy.c ../../msgq.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
msgq_get.o :: msgq_get.c ../../msgq.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho ../../.dep/debug.ho
msgq_init.o :: msgq_init.c ../../msgq.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
msgq_post.o :: msgq_post.c ../../msgq.c ../tap/.dep/basic.ho ../mock_pthread/.dep/mock_pthread.ho
../../msgq.c :: ../../.dep/msgq.ho

# Include file nested dependencies

../mock_pthread/.dep/mock_pthread.ho :: ../mock_pthread/mock_pthread.h
	@if [ ! -d ../mock_pthread/.dep/ ]; then mkdir -p ../mock_pthread/.dep/; fi
	@touch $@

../../.dep/IPMB.ho :: ../../IPMB.h
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/debug.ho :: ../../debug.h ../../.dep/IPMB.ho
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../../.dep/msgq.ho :: ../../msgq.h ../../.dep/IPMB.ho
	@if [ ! -d ../../.dep/ ]; then mkdir -p ../../.dep/; fi
	@touch $@

../tap/.dep/basic.ho :: ../tap/basic.h
	@if [ ! -d ../tap/.dep/ ]; then mkdir -p ../tap/.dep/; fi
	@touch $@
