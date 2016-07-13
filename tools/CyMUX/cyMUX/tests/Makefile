#   Copyright (c) 2015
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the Test stubs/cases
#
#   Uses c-tap-harness https://www.eyrie.org/~eagle/software/c-tap-harness/
#
#   Originating Author: Brian Kurle
#   Creation Date:      18 Dec 2015
#
#   History:
#   18-Dec-2015 bjk - Initial creation

all:
	cd debug_stub ; make
	cd IPMB ; make
	cd mock_pthread ; make
	cd ll ; make
	cd msgq ; make

clean:
	cd debug_stub ; make clean
	cd IPMB ; make clean
	cd mock_pthread ; make clean
	cd ll ; make clean
	cd msgq ; make clean

refresh:
	cd debug_stub ; make refresh
	cd IPMB ; make refresh
	cd mock_pthread ; make refresh
	cd ll ; make refresh
	cd msgq ; make refresh

tests: check

check: TESTS
	runtests -l TESTS

TESTS: all
	ls -1 IPMB/*-t >TESTS
	ls -1 mock_pthread/*-t >>TESTS
	ls -1 ll/*-t >>TESTS
	ls -1 msgq/*-t >>TESTS

over: clean all
