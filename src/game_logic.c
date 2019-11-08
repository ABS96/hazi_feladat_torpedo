#pragma once

#include "game_logic.h"
#include "hardware_control.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
	select,
	debug
} gameState;

static int shots;
static int selected;
static ship ships[NUMBER_OF_LOCATIONS];
static gameState state;

void initializeGame() {
	// Haj�k elhelyez�se v�letlenszer�en
	srand(time(NULL)); // #TODO: legyen random a gener�l�s
	for (int i = 0; i < NUMBER_OF_SHIPS_PER_GAME; ++i) {
		int location;
		do {
			location = rand() % (NUMBER_OF_LOCATIONS / 2);
		} while (ships[location].exists == true); // #TODO: ne �rjenek �ssze a haj�k
		ships[location].exists = true;
	}

	clearDisplay();

	selected = 0;

	shots = 0;
	displayShots(shots);

	state = select;
}

void gameManager() {
	static int cnt = 0;
	static bool blinkState;
	switch (state) {
		case select:
				// egyel�re szimul�ljuk a l�ptet�st a cnt seg�dv�ltoz�val
				blinkState = !blinkState;
				displayShip(selected, blinkState, blinkState);
				delay();
				if(cnt % 6 == 0)
					selectShip(1);
				++cnt;
			break;
		case debug:
				for (int i = 0; i < NUMBER_OF_LOCATIONS; ++i) {
					displayShip(i, ships[i].exists, ships[i].exists);
				}
				delay();
			break;
		default:
			break;
	}
}

void selectShip(int dir) {
	// Legut�bb villogtatott haj� vissza�ll�t�sa
	displayShip(selected, ships[selected].destroyed[0], ships[selected].destroyed[1]);

	// L�ptet�s a t�mb�n
	if (dir > 0) {
		selected = selected < (NUMBER_OF_LOCATIONS - 1) ? selected + 1 : 0;
	} else if (dir < 0) {
		selected = selected > 0 ? selected - 1 : (NUMBER_OF_LOCATIONS) - 1;
	}
}

bool fireTorpedo(int location) {
	// #TODO
}
