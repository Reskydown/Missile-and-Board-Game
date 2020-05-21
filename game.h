#ifndef GAME
#define GAME

#include "missile.h"
#include "ship.h"

typedef struct game {
	int width;
	int height;

	Ship* ships;
	int numShip;

	/*
	 * a matrix of numbers, -1 for no ship,
	 * 0 for the first ship,
	 * 1 for the second ship,
	 * ...
	 */
	int** board;

	/*
	 * a matrix of number, each cell is 0(not attacked) or 1(attacked)
	 */
	int** attack;

	Missile* missileList; /* linked list of missile*/
	Missile* currentMissile; /* the current missile used*/
} Game;

/*
 * load board.txt
 * return 1 if board file is in correct format, otherwise return 0
 */
int loadBoard(Game *g, char* filename);

/*
 * load missiles.txt
 * return 1 if missiles file is in correct format, otherwise return 0
 */
int loadMissile(Game *g, char* filename);

/* reset the game status, used when playing a new game*/
void resetGame(Game *g);

/* convert a string to lower case*/
void toLower(char* str);

void displayGame(Game *g);

int allDestroyed(Game* g);

void playGame(Game* g);

void listAllMissiles(Game *g);

void freeGame(Game* g);

void createBoardFile();

void createMissileFile();
#endif
