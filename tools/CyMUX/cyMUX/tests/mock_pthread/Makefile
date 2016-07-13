#   Copyright (c) 2015
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the cyMux daemon and the cytool CLI
#
#   Originating Author: Brian Kurle
#   Creation Date:      02 Nov 2015
#
#   History:
#   02-Nov-2015 bjk - Initial creation
#   17-Dec-2015 bjk - Refactored to use ll instead of bt

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

TARGETS  = mock_pthread.o 01-init-t 02-destroy-t 03-lock-t 04-unlock-t 05-interact-t 06-cond_init-t 07-cond_destroy-t


all: .dep/Makefile $(TARGETS)

release: cyMUX-$(CYMUX_VERSION)-0.x86_64.rpm
	rsync -aSHP $< rg2:

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
	../../dep.pl

# Target binaries to build
01-init-t: init.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

02-destroy-t: destroy.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

03-lock-t: lock.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

04-unlock-t: unlock.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

05-interact-t: interact.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

06-cond_init-t: cond_init.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

07-cond_destroy-t: cond_destroy.o mock_pthread.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

### Generated targets ###
# Generated via ../../dep.pl
# Object file nested dependencies
cond_destroy.o :: cond_destroy.c .dep/mock_pthread.ho
cond_init.o :: cond_init.c .dep/mock_pthread.ho
destroy.o :: destroy.c .dep/mock_pthread.ho
init.o :: init.c .dep/mock_pthread.ho
interact.o :: interact.c .dep/mock_pthread.ho
lock.o :: lock.c .dep/mock_pthread.ho
mock_pthread.o :: mock_pthread.c .dep/mock_pthread.ho
unlock.o :: unlock.c .dep/mock_pthread.ho

# Include file nested dependencies
.dep/mock_pthread.ho :: mock_pthread.h
