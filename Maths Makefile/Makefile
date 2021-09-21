all: makeObjectFiles makeSharedLib compile

makeObjectFiles: add.c subtract.c
	gcc -Wall -Wextra -pedantic -std=c11 -fPIC -c add.c -o add.o
	gcc -Wall -Wextra -pedantic -std=c11 -fPIC -c subtract.c -o subtract.o
	
makeSharedLib: add.o subtract.o
	gcc -Wall -Wextra -pedantic -std=c11 -shared add.o subtract.o -o libmaths.so

compile: libmaths.so
	gcc -Wall -Wextra -pedantic -std=c11 maths_prog.c -L. -lmaths -o maths_prog

clean:  
	rm -rf add.o subtract.o libmaths.so maths_prog
