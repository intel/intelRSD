========================================================================================
                      Cypress Semiconductor Corporation
                            CyUSB Serial library.
=========================================================================================
Pre-requisite:
1. libusb-1.0.9 is required for compilation and functioning of the APIs in the library.
2. GNU Make and the GCC tool-chain are used for the build process.

Installation steps:

1.cd to the main directory where the library source files are extracted and goto ./linux/library

2.Type 'sudo make'.

2.This will generate shared library libcyusbserial.so.1 and its soft link libcyusbserial.so.
  Both of them will be copied/installed to /usr/local/lib.

3.Go to ./linux/testUtility (I2C/SPI test utility program) and type 'sudo make'. 
This will generate CyUSBSerialTestUtility binary (Refer to README in testUtility folder).

4.User can launch the application and test the communication with Cypress USB-Serial DVk in vendor mode.

Changing USB device node permission:

By default the USB device node created in linux do not have read/write permission
for normal user. In order to make the USB device node accessable, copy 90-cyusb.rules
which is inside the library folder of release package to /etc/udev/rules.d folder (Default VID mentioned is Cypress VID
which is 0x4B4, if VID is changed then update it in ATTR{idVendor} section of rules file).

*** IMPORTANT:
Some distribution of linux have libusb (older version) as part of their package. Uninstall 
the older version and update it with libusb-1.0.9.

**Note:
Refer to ./common/doc/CyUSBSerial API documentation.pdf for description of all
the vendor mode APIs. The header file of the library is in ./common/header/CyUSBSerial.h.
