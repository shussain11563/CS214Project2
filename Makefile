compare: compare.o 
	gcc -g -Wvla -Wall -fsanitize=address,undefined -o compare compare.o

compare.o: compare.c compare.h 
	gcc -c -g -Wvla -Wall -fsanitize=address,undefined compare.c

compare.h: 
	gcc -c -g -Wvla -Wall -fsanitize=address,undefined compare.c

clean:
	rm -f *.o compare
