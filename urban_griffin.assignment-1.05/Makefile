all: pokegame

pokegame: map.o world.o character.o heap.o path.o pokegame.o
	gcc -g map.o world.o character.o heap.o path.o pokegame.o -o pokegame -lncurses

map.o: map.c map.h character.h heap.h
	gcc -Wall -Werror -g map.c -c

world.o: world.c world.h map.h character.h
	gcc -Wall -Werror -g world.c -c

character.o: character.c character.h map.h world.h
	gcc -Wall -Werror -g character.c -c

heap.o: heap.c heap.h character.h
	gcc -Wall -Werror -g heap.c -c

path.o: path.c path.h map.h character.h world.h heap.h
	gcc -Wall -Werror -g path.c -c

pokegame.o: pokegame.c map.h world.h character.h path.h heap.h
	gcc -Wall -Werror -g pokegame.c -c

clean:
	rm -f *.o pokegame *~
