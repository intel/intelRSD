mrgenfile: link
	gcc -c -I ../../../src/include/ main.c -o main.o
	gcc -c -I ../../../src/include/ file.c -o file.o
	gcc -c -I ../../../src/include/ json.c -o json.o
	gcc -c -I ../../../src/include/ snprintf_support.c -o snprintf_support.o
	gcc -c -I ../../../src/include/ msg_reg.c -o msg_reg.o -D MR_GENFILE
	gcc -o $@ main.o msg_reg.o json.o file.o snprintf_support.o
	rm -f *.o file.c  json.c  msg_reg.c snprintf_support.c safeclib_private.h safe_str_constraint.h

.PHONY: link clean
link: 
	ln -fs ../../../src/lib/libutils/file.c file.c
	ln -fs ../../../src/lib/libjson/json.c
	ln -fs ../../../src/lib/libredfish/msg_reg.c
	ln -fs ../../../src/lib/libsecurec/snprintf_support.c
	ln -fs ../../../src/lib/libsecurec/safeclib_private.h
	ln -fs ../../../src/lib/libsecurec/safe_str_constraint.h

clean:
	rm -f mrgenfile *.o file.c  json.c  msg_reg.c
