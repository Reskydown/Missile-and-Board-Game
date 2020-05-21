#ifndef MISSLE
#define MISSLE

struct game;

typedef void (*MissleHandler)(struct game *, int, int);

typedef struct missile {
	char type[20];

	MissleHandler handler;

	struct missile* next;
} Missile;

/* the missile type must be one of single, splash, v-line or h-line*/
int checkMissile(char* missileType);

/* create missile, assume the type is in right format*/
Missile* createMissile(char* missileType);

void addMissile(struct game *g, char* missileType);

/* attack a point*/
void attack(struct game *g, int row, int col);

int getListLength(Missile* node);

void freeMissiles(Missile* node);

#endif
