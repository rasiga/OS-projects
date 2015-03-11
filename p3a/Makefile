all: main

main: main.c libmem.so
	gcc -lmem -L . -o main main.c -Wall -Werror -pthread

libmem.so: mem.o
	gcc -shared -o libmem.so mem.o -Wall -Werror -pthread

mem.o: mem.c
	gcc -c -fpic mem.c -o mem.o -Wall -Werror -pthread

clean:
	rm -rf *.so *.o main



