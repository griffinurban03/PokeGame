all: pokegame

pokegame: map.o world.o character.o pokegame.o
	gcc -g map.o world.o character.o pokegame.o -o pokegame

map.o: map.c map.h character.h
	gcc -Wall -Werror -g map.c -c

world.o: world.c world.h map.h
	gcc -Wall -Werror -g world.c -c

character.o: character.c character.h map.h
	gcc -Wall -Werror -g character.c -c

pokegame.o: pokegame.c map.h world.h
	gcc -Wall -Werror -g pokegame.c -c

clean:
	rm -f *.o pokegame *~
