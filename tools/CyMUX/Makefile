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

LIBDIR=lib
BINDIR=bin

CYLIB=libcyusbserial.so.1
CYLIBDIR=CyUSBSerial_SDK_Linux/linux/library
LIBOBJ=$(CYLIBDIR)/$(CYLIB)
LIBTGT=$(LIBDIR)/$(CYLIB)

CYDIR=cyMUX
CYOBJS  = $(CYDIR)/cyMUX
CYOBJS += $(CYDIR)/cytool
CYTGTS  = $(BINDIR)/cyMUX
CYTGTS += $(BINDIR)/cytool

all: $(LIBTGT) $(CYTGTS)

over: clean all

distclean: 
	rm -rf $(LIBDIR) $(BINDIR)
	cd $(CYLIBDIR) ; make distclean
	cd $(CYDIR) ; make distclean

clean:
	cd $(CYLIBDIR) ; make clean
	cd $(CYDIR) ; make clean

$(LIBTGT) :: $(LIBOBJ)
	mkdir -p $(LIBDIR)
	cd $(CYLIBDIR) ; make
	cp $(LIBOBJ) $(LIBDIR)

$(CYTGTS) :: $(CYOBJS)
	mkdir -p $(BINDIR)
	cd $(CYDIR) ; make
	cp $+ $(BINDIR)

$(LIBOBJ):
	cd $(CYLIBDIR) ; make

$(CYOBJS):
	cd $(CYDIR) ; make
