#pragma once

#include <stdbool.h>

#define NUMBER_OF_SHIPS_PER_GAME 4
#define NUMBER_OF_LOCATIONS 28

typedef struct {
	bool exists;
	bool destroyed[2];
} ship;

void initializeGame();
void selectShip(int dir);
bool fireTorpedo(int location);
