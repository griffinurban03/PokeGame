# This is a makefile
map: map.o pokegame.o
	gcc map.o pokegame.o -o pokegame

map.o: map.c map.h
	gcc -Wall -Werror -g map.c -c

pokegame.o: pokegame.c map.h
	gcc -Wall -Werror -g pokegame.c -c

clean:
	rm -f *.o pokegame *~