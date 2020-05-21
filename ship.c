#include <string.h>

#include "ship.h"
#include "game.h"

/* check if the location is in correct format */
int checkLocation(char* location, int boardWidth, int boardHeight) {
	int valid = 1;
	if (strlen(location) == 2) {
		int col = location[0] - 'a';
		int row = location[1] - '1';

		valid = row >= 0 && col >= 0 && row < boardHeight && col < boardWidth;
	} else {
		valid = 0;
	}
	return valid;
}

/* check the ship if out of board, or overlap with other ship*/
int checkLength(Game *g, char* location, char* direction, int length) {
	int i, j;
	int count = 0;
	int valid = 1;
	int dirRow, dirCol;

	int col = location[0] - 'a';
	int row = location[1] - '1';
	i = row;
	j = col;

	if (strcmp("n", direction) == 0) {
		dirCol = 0;
		dirRow = 1;
	} else if (strcmp("s", direction) == 0) {
		dirCol = 0;
		dirRow = -1;
	} else if (strcmp("w", direction) == 0) {
		dirCol = 1;
		dirRow = 0;
	} else {
		dirCol = -1;
		dirRow = 0;
	}

	while (i >= 0 && j >= 0 && i < g->height && j < g->width && count < length
			&& g->board[i][j] < 0) {
		i += dirRow;
		j += dirCol;
		count++;
	}

	if (count != length) {
		valid = 0;
	}

	return valid;
}

/* check if the direction is in correct format */
int checkDirection(char* direction) {
	return strcmp("n", direction) == 0 || strcmp("e", direction) == 0
			|| strcmp("s", direction) == 0 || strcmp("w", direction) == 0;
}

/* set the direction of a ship, assume the direction is in right format*/
void setDirection(Ship* s, char* direction) {
	if (strcmp("n", direction) == 0) {
		s->dirCol = 0;
		s->dirRow = 1;
	} else if (strcmp("s", direction) == 0) {
		s->dirCol = 0;
		s->dirRow = -1;
	} else if (strcmp("w", direction) == 0) {
		s->dirCol = 1;
		s->dirRow = 0;
	} else {
		s->dirCol = -1;
		s->dirRow = 0;
	}
}

