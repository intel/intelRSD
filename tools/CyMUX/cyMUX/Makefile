#   Copyright (c) 2015, 2016 Intel Corporation
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#   Copyright (c) 2015, 2016
#   Intel Corporation.  All Rights Reserved
#
#   Purpose:
#
#   Build the cyMux daemon and the cytool CLI
#
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
CFLAGS += -pthread -g
CFLAGS += -D_GNU_SOURCE
CYMUX_VERSION=1.07
CFLAGS += -D__CYMUX_VERSION__=$(CYMUX_VERSION)
CFLAGS += -D__CYTOOL_VERSION__=2.02

LDFLAGS  = -lcyusbserial -L ../CyUSBSerial_SDK_Linux/linux/library
LDFLAGS += -lpthread

TARGETS  = cyMUX cytool test_client test_responder

# cyMUX object dependencies
MUX_OBJECTS  = cyMUX.o
MUX_OBJECTS += mux_usage.o
MUX_OBJECTS += receiver_thread.o
MUX_OBJECTS += sender_thread.o
MUX_OBJECTS += worker_thread.o
MUX_OBJECTS += connection_manager.o
MUX_OBJECTS += cy_i2c.o
MUX_OBJECTS += msgq.o
MUX_OBJECTS += IPMB.o
MUX_OBJECTS += ll.o
MUX_OBJECTS += debug.o
MUX_OBJECTS += sys_send.o

# cytool object dependencies
TOOL_OBJECTS  = cytool.o

TEST_CLIENT_OBJECTS  = test_client.o
TEST_CLIENT_OBJECTS += test_client_usage.o
TEST_CLIENT_OBJECTS += IPMB.o
TEST_CLIENT_OBJECTS += debug.o
TEST_CLIENT_OBJECTS += sys_send.o

TEST_RESPONDER_OBJECTS  = test_responder.o
TEST_RESPONDER_OBJECTS += test_responder_usage.o
TEST_RESPONDER_OBJECTS += IPMB.o
TEST_RESPONDER_OBJECTS += debug.o
TEST_RESPONDER_OBJECTS += sys_send.o

all: .dep/Makefile $(TARGETS)
	cd tests ; make all

release: cyMUX-$(CYMUX_VERSION)-0.x86_64.rpm

.dep/Makefile: Makefile
	make clean
	@if [ ! -d .dep ]; then mkdir -p .dep; fi
	@touch $@

clean:
	rm -rf $(TARGETS) $(MUX_OBJECTS) $(TOOL_OBJECTS) $(TEST_RESPONDER_OBJECTS) $(TEST_CLIENT_OBJECTS) .dep *.rpm
	cd tests ; make clean

distclean: clean
	rm -rf CyUSBSerial.h libcyusbserial*

over: clean all

lines:
	wc -l *\.c *\.h

tests: .dep/Makefile $(TARGETS)
	cd tests ; make all

check: tests
	runtests  -b tests -s tests -l tests/TESTS

refresh:
	./dep.pl
	cd tests ; make refresh

TESTS: tests
	cd tests ; make TESTS

# Target binaries to build
cyMUX: $(MUX_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(MUX_OBJECTS) $(LDFLAGS)

cytool: $(TOOL_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TOOL_OBJECTS) $(LDFLAGS)

test_client: $(TEST_CLIENT_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TEST_CLIENT_OBJECTS) $(LDFLAGS)

test_responder: $(TEST_RESPONDER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(TEST_RESPONDER_OBJECTS) $(LDFLAGS)

cyMUX-$(CYMUX_VERSION)-0.x86_64.rpm: all cyMUX.service cyMUX.spec mkrelease.sh
	CYMUX_VERSION=$(CYMUX_VERSION) ./mkrelease.sh

### Generated targets ###
# Generated via ./dep.pl
# Object file nested dependencies
connection_manager.o :: connection_manager.c .dep/connection_manager.ho .dep/sender_thread.ho .dep/worker_thread.ho .dep/configs.ho .dep/shim.ho .dep/ll.ho .dep/debug.ho
cy_i2c.o :: cy_i2c.c .dep/cy_i2c.ho .dep/configs.ho .dep/debug.ho
cyMUX.o :: cyMUX.c .dep/connection_manager.ho .dep/receiver_thread.ho .dep/sender_thread.ho .dep/worker_thread.ho .dep/mux_usage.ho .dep/cy_i2c.ho .dep/configs.ho .dep/stringify.ho .dep/debug.ho
cytool.o :: cytool.c .dep/IPMB.ho .dep/configs.ho .dep/stringify.ho
debug.o :: debug.c .dep/debug.ho
IPMB.o :: IPMB.c .dep/IPMB.ho .dep/configs.ho .dep/debug.ho .dep/shim.ho
ll.o :: ll.c .dep/ll.ho .dep/IPMB.ho .dep/debug.ho .dep/shim.ho
msgq.o :: msgq.c .dep/msgq.ho
mux_usage.o :: mux_usage.c .dep/mux_usage.ho .dep/debug.ho .dep/stringify.ho
receiver_thread.o :: receiver_thread.c .dep/receiver_thread.ho .dep/worker_thread.ho .dep/configs.ho .dep/cy_i2c.ho .dep/debug.ho
sender_thread.o :: sender_thread.c .dep/sender_thread.ho .dep/worker_thread.ho .dep/cy_i2c.ho .dep/configs.ho .dep/debug.ho
sys_send.o :: sys_send.c .dep/shim.ho
test_client.o :: test_client.c .dep/test_client_usage.ho .dep/configs.ho
test_client_usage.o :: test_client_usage.c .dep/test_client_usage.ho
test_responder.o :: test_responder.c .dep/test_responder_usage.ho .dep/IPMB.ho .dep/configs.ho .dep/debug.ho .dep/shim.ho
test_responder_usage.o :: test_responder_usage.c .dep/test_responder_usage.ho .dep/debug.ho
worker_thread.o :: worker_thread.c .dep/worker_thread.ho .dep/sender_thread.ho .dep/connection_manager.ho .dep/configs.ho .dep/ll.ho .dep/debug.ho

# Include file nested dependencies
.dep/configs.ho :: configs.h
.dep/connection_manager.ho :: connection_manager.h .dep/IPMB.ho .dep/msgq.ho
.dep/cy_i2c.ho :: cy_i2c.h
