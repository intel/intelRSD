LIB_DIR=/opt/rsa/module_firmware/lib64
UDEV_ETC=/etc/udev/rules.d
CC=gcc
CFLAGS=-fPIC -g -Wall -I ../../common/header
OBJECTS=cyusb.o cyuart.o cyi2c.o cyspi.o cyphdc.o cyjtag.o cymisc.o

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $*.o

all: libcyusbserial.so

install:
	mkdir -p $(LIB_DIR) $(UDEV_ETC)
	install -m 644 90-cyusb.rules $(UDEV_ETC)
	install -m 755 libcyusbserial.so.1 $(LIB_DIR)
	ln -sf libcyusbserial.so.1 $(LIB_DIR)/libcyusbserial.so

release: all
	./mkrelease.sh

uninstall:
	rm -f $(LIB_DIR)/libcyusbserial.so* $(UDEV_ETC)/90-cyusb.rules

clean:
	rm -f $(OBJECTS) libcyusbserial.so* *rpm

over: clean all

distclean: clean

libcyusbserial.so: libcyusbserial.so.1
	ln -s libcyusbserial.so.1 libcyusbserial.so

libcyusbserial.so.1: $(OBJECTS)
	gcc -shared -g -Wl,-soname,$@ -o $@ $(OBJECTS) -lusb-1.0
	rm -f ../../../cyMUX/libcyusbserial.so* ../../../cyMUX/CyUSBSerial.h
	cp libcyusbserial.so.1 ../../../cyMUX
	ln -s libcyusbserial.so.1 ../../../cyMUX/libcyusbserial.so
	ln -s ../CyUSBSerial_SDK_Linux/common/header/CyUSBSerial.h ../../../cyMUX/

