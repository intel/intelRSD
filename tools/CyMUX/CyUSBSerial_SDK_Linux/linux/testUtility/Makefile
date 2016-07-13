all:
	gcc -g -o CyUSBSerialTestUtility Test_Utility.c -L ../../../cyserial/lib -lcyusbserial -w
	cp 90-cyusb.rules /etc/udev/rules.d 
	cp CyUSBSerialTestUtility /usr/bin
	cp CyUSBSerial.sh /usr/bin
	chmod 777 /usr/bin/CyUSBSerial.sh
clean:
	rm -f CyUSBSerialTestUtility
help:
	@echo	'make		would compile and create the library and create a link'
	@echo	'make clean	would remove the library and the soft link to the library (soname)'
