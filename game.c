#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BLUE "\033[1;34m"
#define RESET "\033[0m"
#define MAGENTA "\033[1;35m"

#include "game.h"

/* convert a string to lower case*/
void toLower(char* str) {
	int i, len;
	len = strlen(str);
	for (i = 0; i < len; i++) {
		if (str[i] >= 'A' && str[i] <= 'Z') {
			str[i] += 'a' - 'A';
		}
	}
}

/*
 * load board.txt
 * return 1 if board file is in correct format, otherwise return 0
 */
int loadBoard(Game *g, char* filename) {
	FILE *fp;

	int i, j;
	int lineNum;
	int status;
	int capacity;
	int correct = 1;
	char errorMsg[100];

	fp = fopen(filename, "r");
	if (fp) {
		g->width = 0;
		g->height = 0;
		status = fscanf(fp, "%d%*c%d", &g->width, &g->height);

		if (g->width >= 1 && g->height >= 1 && g->width <= 12
				&& g->height <= 12) {
			char buffer[100];

			/* allocate matrix*/
			g->board = (int**) malloc(sizeof(int*) * g->height);
			g->attack = (int**) malloc(sizeof(int*) * g->height);
			for (i = 0; i < g->height; i++) {
				g->board[i] = (int*) malloc(sizeof(int) * g->width);
				g->attack[i] = (int*) malloc(sizeof(int) * g->width);
				for (j = 0; j < g->width; j++) {
					g->board[i][j] = -1; /*default, no ship on each cell*/
					g->attack[i][j] = 0; /*default, each cell is not attacked*/
				}
			}

			/* load ships*/
			g->numShip = 0;
			capacity = 1;
			g->ships = (Ship*) malloc(sizeof(Ship) * capacity);

			/* load each ship : D3 N 3 tabi*/
			fgets(buffer, 100, fp);
			lineNum = 2;
			while (correct && fgets(buffer, 100, fp)) {
				char location[20];
				char direction[20];
				int length;
				char shipName[50];

				strcpy(location, "");
				strcpy(direction, "");
				length = 0;
				strcpy(shipName, "");

				status = sscanf(buffer, "%s%s%d%s", location, direction,
						&length, shipName);

				if (status == 4) {
					char* token;
					int count;

					toLower(location);
					toLower(direction);

					/* the above sscanf just read the first word of ship name*/
					/* now read all the words*/
					token = strtok(buffer, " \t");
					count = 1;
					while (token != NULL) {
						if (count > 4) {
							strcat(shipName, " ");
							strcat(shipName, token);
						}
						token = strtok(NULL, " \t");
						count++;
					}

					if (!checkLocation(location, g->width, g->height)) {
						correct = 0;
						sprintf(errorMsg,
								"file %s has invalid ship location at line %d.\n",
								filename, lineNum);
					} else if (!checkDirection(direction)) {
						correct = 0;
						sprintf(errorMsg,
								"file %s has invalid ship direction at line %d.\n",
								filename, lineNum);
					} else if (length <= 0
							|| !checkLength(g, location, direction, length)) {
						correct = 0;
						sprintf(errorMsg,
								"file %s has invalid ship length at line %d.\n",
								filename, lineNum);
					} else {
						/* ship is correct */
						Ship s;
						s.hitNum = 0;
						s.col = location[0] - 'a';
						s.row = location[1] - '1';

						setDirection(&s, direction);
						s.length = length;

						strcpy(s.name, shipName);

						/* add ship*/
						if (g->numShip >= capacity) {
							Ship *temp;

							capacity += 1;
							temp = (Ship*) malloc(sizeof(Ship) * capacity);

							for (i = 0; i < g->numShip; i++) {

								temp[i] = g->ships[i];
							}

							free(g->ships);
							g->ships = temp;
						}

						g->ships[g->numShip++] = s;
						resetGame(g);
					}
				} else {
					correct = 0;
					sprintf(errorMsg, "file %s has invalid ship.\n", filename);
				}

				lineNum++;
			}

			/*set no missiles now*/
			g->missileList = NULL;
			g->currentMissile = NULL;
		} else {
			correct = 0;
			sprintf(errorMsg, "the size of board is incorrect.\n");
		}
		fclose(fp);
	} else {
		correct = 0;
		sprintf(errorMsg, "not found file %s.\n", filename);
	}

	if (!correct) {
		printf("%s\n", errorMsg);
	}
	return correct;
}

/*
 * load missiles.txt
 * return 1 if missiles file is in correct format, otherwise return 0
 */
int loadMissile(Game *g, char* filename) {
	FILE *fp;

	int lineNum;
	int correct = 1;
	char errorMsg[100];

	/*set no missiles at first*/
	g->missileList = NULL;
	g->currentMissile = NULL;

	fp = fopen(filename, "r");
	if (fp) {
		char buffer[100];
		char type[30];

		lineNum = 1;
		while (correct && fgets(buffer, 100, fp)) {
			if (sscanf(buffer, "%s", type) == 1) {
				if (!checkMissile(type)) {
					correct = 0;
					sprintf(errorMsg,
							"file %s has invalid missile at line %d.\n",
							filename, lineNum);
				} else {
					addMissile(g, type);
				}
			} else {
				correct = 0;
				sprintf(errorMsg, "file %s has invalid missile at line %d.\n",
						filename, lineNum);
			}
			lineNum++;
		}
		fclose(fp);
	} else {
		correct = 0;
		sprintf(errorMsg, "not found file %s.\n", filename);
	}

	if (!correct) {
		printf("%s\n", errorMsg);
	}
	return correct;
}

/* reset the game status, used when playing a new game*/
void resetGame(Game *g) {
	int i, j, k;

	for (i = 0; i < g->height; i++) {
		for (j = 0; j < g->width; j++) {
			g->board[i][j] = -1; /*default, no ship on each cell*/
			g->attack[i][j] = 0; /*default, each cell is not attacked*/
		}
	}

	for (k = 0; k < g->numShip; k++) {
		int count = 0;

		g->ships[k].hitNum = 0;
		i = g->ships[k].row;
		j = g->ships[k].col;
		while (i >= 0 && j >= 0 && i < g->height && j < g->width
				&& count < g->ships[k].length) {
			g->board[i][j] = k;
			i += g->ships[k].dirRow;
			j += g->ships[k].dirCol;
			count++;
		}
	}

	g->currentMissile = g->missileList;
}

void listAllMissiles(Game *g) {
	Missile* node = g->missileList;
	while (node != NULL) {
		printf("%s\n", node->type);
		node = node->next;
	}
}

int allDestroyed(Game* g) {
	int i;
	int all = 1;
	for (i = 0; i < g->numShip; i++) {
		if (g->ships[i].hitNum < g->ships[i].length) {
			all = 0;
		}
	}
	return all;
}

void displayGame(Game *g) {
	int i, j;

	printf("\n");
	printf(":) |");
	for (i = 0; i < g->width; i++) {
		printf("| %c ", 'A' + i);
	}
	printf("|\n");

	printf("---+");
	for (j = 0; j < g->width; j++) {
		printf("+===");
	}
	printf("+\n");

	for (i = 0; i < g->height; i++) {
		printf("%2d |", i + 1);
		for (j = 0; j < g->width; j++) {
			char ch = '#';
			if (g->attack[i][j]) {
				if (g->board[i][j] >= 0) {
					ch = '0';
				} else {
					ch = 'X';
				}
			} else {
				/* not attack*/
			}

			printf("| ");

#ifdef MONO
			printf("%c", ch);
#else
			if (ch == '#') {
#ifdef DEBUG
				if(g->board[i][j] >= 0) {
					printf(RED "%c" RESET, ch);
				} else {
					printf(BLUE "%c" RESET, ch);
				}
#else
				printf(BLUE "%c" RESET, ch);
#endif

			} else if (ch == '0') {
				printf(GREEN "%c" RESET, ch);
			} else if (ch == 'X') {
				printf(RED "%c" RESET, ch);
			}
#endif

			printf(" ");

		}
		printf("|\n");

		printf("---+");
		for (j = 0; j < g->width; j++) {
			printf("+---");
		}
		printf("+\n");
	}

	printf("\nMissiles Left: %d\n", getListLength(g->currentMissile));
	printf("Current Missile: %s\n",
			g->currentMissile == NULL ? "None" : g->currentMissile->type);
}

void playGame(Game* g) {
	char location[20];

	resetGame(g);

	while (!allDestroyed(g) && getListLength(g->currentMissile) != 0) {
		displayGame(g);

		printf("\nEnter Next Target: ");
		scanf("%s", location);
		toLower(location);

		if (!checkLocation(location, g->width, g->height)) {
			printf("Invalid Target\n\n");
		} else {
			int col = location[0] - 'a';
			int row = location[1] - '1';

			g->currentMissile->handler(g, row, col);
			g->currentMissile = g->currentMissile->next;
		}
	}

	displayGame(g);
	if (allDestroyed(g)) {
		printf("\nYou Win!\n\n");
	} else {
		printf("\nYou Lose!\n\n");
	}
}

void freeGame(Game* g) {
	int i;
	for (i = 0; i < g->height; i++) {
		free(g->board[i]);
		free(g->attack[i]);
	}
	free(g->board);
	free(g->attack);

	free(g->ships);
	freeMissiles(g->missileList);
}

/*
 * remove the '\n' in the rear of a string
 */
void trim(char* str) {
	int len = strlen(str);
	while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
		str[len - 1] = '\0';
		len = strlen(str);
	}
}

void createBoardFile() {
	FILE* fp;
	int width, height;
	int quit = 0;

	char filename[256];

	printf("enter width: ");
	scanf("%d", &width);

	printf("enter height: ");
	scanf("%d", &height);

	if (width >= 1 && height >= 1 && width <= 12 && height <= 12) {
		printf("enter the filename: ");
		scanf("%s", filename);

		fp = fopen(filename, "w");

		fprintf(fp, "%d,%d\n", width, height);
		while (!quit) {
			char location[50];
			char direction[50];
			int length;
			char shipName[50];
			char option[50];

			printf("enter the start location of ship: ");
			scanf("%s", location);
			toLower(location);

			if (checkLocation(location, width, height)) {
				printf("enter the direction of ship: ");
				scanf("%s", direction);
				toLower(direction);

				if (checkDirection(direction)) {
					printf("enter the length of ship: ");
					scanf("%d", &length);

					if (length >= 1) {
						printf("enter the name of ship: ");
						fgets(shipName, 50, stdin); /* to accept the '\n' of last input*/
						fgets(shipName, 50, stdin);
						trim(shipName);

						fprintf(fp, "%s %s %d %s\n", location, direction,
								length, shipName);
					} else {
						printf("invalid length.\n");
					}
				} else {
					printf("invalid direction.\n");
				}
			} else {
				printf("invalid location.\n");
			}

			printf("continue to add ship?(y/n): ");
			scanf("%s", option);
			if (strcmp("y", option) != 0 && strcmp("Y", option) != 0) {
				quit = 1;
			}
		}

		fclose(fp);
	} else {
		printf("invalid width or height.\n");
	}
}

void createMissileFile() {
	FILE* fp;
	int quit = 0;

	char filename[256];
	printf("enter the filename: ");
	scanf("%s", filename);

	fp = fopen(filename, "w");

	while (!quit) {
		char type[50];
		char option[50];

		printf("enter the type of missile: ");
		scanf("%s", type);

		if (checkMissile(type)) {
			fprintf(fp, "%s\n", type);
		} else {
			printf("invalid missile.\n");
		}

		printf("continue to add missile?(y/n): ");
		scanf("%s", option);
		if (strcmp("y", option) != 0 && strcmp("Y", option) != 0) {
			quit = 1;
		}
	}

	fclose(fp);
}

