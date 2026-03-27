main: add.o subtract.o main.o
	gcc add.o subtract.o main.o -o main
add.o: add.c compute.h
	gcc -c add.c -o add.o -Wall -g
subtract.o: subtract.c compute.h
	gcc -c subtract.c -o subtract.o -Wall -g
main.o: main.c compute.h
	gcc -c main.c -o main.o -Wall -g
clean: 
	rm -f main main.o add.o subtract.o
rebuild: clean main 
