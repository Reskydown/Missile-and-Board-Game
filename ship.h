#ifndef SHIP
#define SHIP

struct game;

typedef struct ship {
	/* position of ship*/
	int row;
	int col;

	/* direction of ship, N: (-1, 0), S:(1, 0), W(0, -1), E(0, 1) */
	int dirRow;
	int dirCol;

	int length; /* length of ship */

	char name[50]; /* name of ship*/

	/* number of hit on this ship.
	 * if hitNum  == length, this ship is destroyed
	 */
	int hitNum;
} Ship;

/* check if the location is in correct format */
int checkLocation(char* location, int boardWidth, int boardHeight);

/* check if the direction is in correct format */
int checkDirection(char* direction);

/* check the ship if out of board, or overlap with other ship*/
int checkLength(struct game *g, char* location, char* direction, int length);

/* set the direction of a ship, assume the direction is in right format*/
void setDirection(Ship* s, char* direction);

#endif
