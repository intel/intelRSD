all:
	gcc -pthread cyipmb.c cppapp.c -o cpp -lcyusbserial -Wall -lcrypto -L ./
clean:
	rm -f cpp
help:
	@echo	'make		would compile and create the library and create a link'
	@echo	'make clean	would remove the library and the soft link to the library (soname)'
