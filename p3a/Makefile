all: main

main: main.c libmem.so
	gcc -g -lmem -L . -o main main.c -Wall -Werror

libmem.so: mem.o
	gcc  -shared -o libmem.so mem.o

mem.o: mem.c
	gcc -c -fpic mem.c -Wall -Werror

clean:
	rm -rf *.so *.o main 
