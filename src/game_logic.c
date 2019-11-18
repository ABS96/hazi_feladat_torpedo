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
static int parts_left;
static int selected;
static ship ships[NUMBER_OF_LOCATIONS / 2];
static gameState state;

void initializeGame() {
	// haj�k t�mbj�nek t�rl�se
	for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
		ships[i].exists = false;
		ships[i].destroyed[0] = false;
		ships[i].destroyed[1] = false;
	}
	// haj�k elhelyez�se v�letlenszer�en
	srand(time(NULL));

	const int max_step = NUMBER_OF_LOCATIONS / (2 * NUMBER_OF_SHIPS_PER_GAME) - 1; // a legnagyobb l�p�s 2 haj� k�z�tt akkora legyen, hogy �gy is elf�rjenek a kijelz�n
	int location = rand() % max_step; // az els� haj� hely�nek kijel�l�se
	ships[location].exists = true;
	for (int i = 1; i < NUMBER_OF_SHIPS_PER_GAME; ++i) {
		location += (rand() % (max_step - 4)) + 4; // legfeljebb max_step, de legal�bb egy karakternyi hely legyen a haj�k k�z�tt
		ships[location].exists = true;
	}

	clearDisplay();

	selected = 0;

	shots = 0;
	displayShots(shots);

	parts_left = NUMBER_OF_SHIPS_PER_GAME * 2;

	state = select;
}

void selectShip(int dir) {
	// Legut�bb villogtatott haj� vissza�ll�t�sa
	displayShip(selected / 2, ships[selected / 2].destroyed[0], ships[selected / 2].destroyed[1]);

	// L�ptet�s a t�mb�n
	if (dir > 0) {
		selected = selected < (NUMBER_OF_LOCATIONS - 1) ? selected + 1 : 0;
	} else if (dir < 0) {
		selected = selected > 0 ? selected - 1 : (NUMBER_OF_LOCATIONS) - 1;
	}
}

void checkWin() {
	if (parts_left == 0) {
		animWin();
		initializeGame();
	}
}

bool fireTorpedo(int location) {
	bool will_be_hit = ships[location / 2].exists && ships[location / 2].destroyed[location % 2] == false;
	animShot();
	if (will_be_hit) {
		ships[location / 2].destroyed[location % 2] = true;
		--parts_left;
		animHit();
		displayShip(location / 2, ships[location / 2].destroyed[0], ships[location / 2].destroyed[0]);
		checkWin();
	}
	return will_be_hit;
}

void gameManager() {
	static bool blink_state;
	int input;
	bool segment0;
	bool segment1;
	switch (state) {
		case select:
				blink_state = !blink_state;
				segment0 = selected % 2 == 0 ? blink_state : ships[selected / 2].destroyed[0];
				segment1 = selected % 2 == 1 ? blink_state : ships[selected / 2].destroyed[1];
				displayShip(selected / 2, segment0, segment1);
				delay();

				input = receiveCharacter();
				switch (input) {
					case 'a': // a: balra
					case 'd': // d: jobbra
						selectShip(input == 'a' ? -1 : 1);
						break;
					case ' ': // space: l�v�s
						fireTorpedo(selected);
						displayShots(++shots);
						break;
					case 'g': // g: haj�k helyzet�nek kijelz�se
						state = debug;
						break;
					default:
						//if (input > 0)
						//	transmitCharacter(input);
						break;
				}
			break;
		case debug:
				for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
					displayShip(i, ships[i].exists, ships[i].exists);
				}
				input = receiveCharacter();
				if (input == 'g') {
					for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
						displayShip(i, ships[i].destroyed[0], ships[i].destroyed[1]);
					}
					state = select;
				}
			break;
		default:
			break;
	}
}
