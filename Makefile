FLAG = -Wall -Wextra

build: bmp_utilities.h bmp_utilities.o main.c
		gcc -g bmp_utilities.o main.c -o bmp $(FLAG)

bmp_utilities.o: bmp_utilities.h bmp_utilities.c
				gcc bmp_utilities.c $(FLAG) -c

run: bmp
	valgrind --tool=memcheck --leak-check=full --error-exitcode=1 ./bmp

clean:
	rm *.o bmp
