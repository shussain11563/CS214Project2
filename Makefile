

compare: compare.o strbuf.o queue.o
	gcc -g -std=c99 -Wvla -Wall -fsanitize=address,undefined -o compare compare.o strbuf.o queue.o
	rm *.o

compare.o: compare.c strbuf.h queue.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined compare.c

queue.o: queue.c strbuf.h queue.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined queue.c

strbuf.o: strbuf.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined strbuf.c



recurse: recurse.o strbuf.o
	gcc -g -std=c99 -Wvla -Wall -fsanitize=address,undefined -o recurse recurse.o strbuf.o

recurse.o: recurse.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined recurse.c

strbuf.o: strbuf.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined strbuf.c

thread : pthreadTest.c
	gcc -o -g -std=c99 -Wvla -Wall -fsanitize=address,undefined pthreadTest.c -lpthread