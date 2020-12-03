binary-search : binary-search.o
	gcc binary-search.o -o binary-search

binary-search.o : binary-search.c
	gcc -c binary-search.c -o binary-search.o -g

clean :
	rm -f binary-search.o
