compare: compare.o strbuf.o queue.o compare.h wfd.h
	gcc -g -pthread -std=c99 -Wvla -Wall -fsanitize=address,undefined -o compare compare.o strbuf.o queue.o -lm
	rm *.o

compare.o: compare.c strbuf.h queue.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined compare.c

queue.o: queue.c strbuf.h queue.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined queue.c

strbuf.o: strbuf.c strbuf.h
	gcc -c -g -std=c99 -Wvla -Wall -fsanitize=address,undefined strbuf.c
