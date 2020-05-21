#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

int main(int argc, char* argv[]) {

	setbuf(stdout, NULL);

	if (argc != 3) {
		printf("Usage: ./battleships [board.txt] [missile.txt]\n");
	} else {
		int quit = 0;

		Game g;
		loadBoard(&g, argv[1]);
		loadMissile(&g, argv[2]);
		while (!quit) {
			char option[20];

			printf("1 Play the Game\n");
			printf("2 List All missiles\n");
			printf("3 Create Board File\n");
			printf("4 Create Missile File\n");
			printf("0 Exit\n");
			scanf("%s", option);
			if (strcmp("1", option) == 0) {
				playGame(&g);
			} else if (strcmp("2", option) == 0) {
				listAllMissiles(&g);
			}  else if (strcmp("3", option) == 0) {
				createBoardFile();
			}  else if (strcmp("4", option) == 0) {
				createMissileFile();
			} else if (strcmp("0", option) == 0) {
				quit = 1;
			} else {
				printf("Invalid option!\n");
			}
		}

		freeGame(&g);
	}
	return 0;
}
