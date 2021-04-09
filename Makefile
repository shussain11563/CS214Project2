recurse: recurse.o strbuf.o
	gcc -g -std=c99 -Wvla -Wall -fsanitize=address,undefined -o recurse recurse.o strbuf.o

recurse.o: recurse.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined recurse.c

strbuf.o: strbuf.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined strbuf.c
