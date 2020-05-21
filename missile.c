#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "game.h"
#include "missile.h"

/* the missile type must be one of single, splash, v-line or h-line*/
int checkMissile(char* missileType) {
	return strcmp("single", missileType) == 0
			|| strcmp("splash", missileType) == 0
			|| strcmp("v-line", missileType) == 0
			|| strcmp("h-line", missileType) == 0;
}

/* attack a point*/
void attack(Game *g, int row, int col) {
	if (row >= 0 && col >= 0 && row < g->height && col < g->width
			&& !g->attack[row][col]) {
		int ship = g->board[row][col];
		g->attack[row][col] = 1;

		if (ship >= 0) {
			g->ships[ship].hitNum++;
			if (g->ships[ship].hitNum >= g->ships[ship].length) {
				printf("You destroy %s\n", g->ships[ship].name);
			}
		}
	}
}

void handleSingle(Game *g, int row, int col) {
	attack(g, row, col);
}

void handleSplash(Game *g, int row, int col) {
	int i, j;
	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			attack(g, row + i, col + j);
		}
	}
}

void handleVLine(Game *g, int row, int col) {
	int i;
	for (i = 0; i < g->height; i++) {
		attack(g, i, col);
	}
}

void handleHLine(Game *g, int row, int col) {
	int i;
	for (i = 0; i < g->width; i++) {
		attack(g, row, i);
	}
}

/* create missile, assume the type is in right format*/
Missile* createMissile(char* missileType) {
	Missile* m = (Missile*) malloc(sizeof(Missile));
	m->next = NULL;
	strcpy(m->type, missileType);

	if (strcmp("single", missileType) == 0) {
		m->handler = handleSingle;
	} else if (strcmp("splash", missileType) == 0) {
		m->handler = handleSplash;
	} else if (strcmp("v-line", missileType) == 0) {
		m->handler = handleVLine;
	} else if (strcmp("h-line", missileType) == 0) {
		m->handler = handleHLine;
	}

	return m;
}

void addMissile(Game *g, char* missileType) {
	Missile* m = createMissile(missileType);
	if (g->missileList == NULL) {
		g->missileList = m;
		g->currentMissile = m;
	} else {
		Missile* tail = g->missileList;
		while (tail->next != NULL) {
			tail = tail->next;
		}

		tail->next = m;
	}
}

int getListLength(Missile* node) {
	int length = 0;
	while (node != NULL) {
		node = node->next;
		length++;
	}
	return length;
}

void freeMissiles(Missile* node) {
	while (node != NULL) {
		Missile* temp = node;
		node = node->next;
		free(temp);
	}
}

