mrgenfile: link
	g++ encrypter.cpp -I../../src/include -fPIC -shared -o librmm_encrypter.so -lcrypt
	gcc main.c -I../../src/include -Wall -o encrypt -L./ -lrmm_encrypter -Wl,-rpath=./ -lrmm_securec

.PHONY: link clean
link:
	ln -s ../../src/lib/libencrypter/encrypter.cpp encrypter.cpp
	(mkdir libencrypter;cd libencrypter; ln -s ../../../src/include/libencrypter/encrypter.hpp encrypter.hpp; cd ..; ln -s ../../build/lib/librmm_securec.so)

clean:
	rm -rf encrypter.cpp libencrypter librmm_encrypter.so encrypt librmm_securec.so
