#include "game_logic.h"
#include "hardware_control.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
	init,
	select,
	debug
} gameState;

static int shots;		// eddigi l�v�sek sz�ma
static int parts_left;	// m�g elpuszt�tand� szegmensek sz�ma
static int selected;	// �ppen akt�v haj�
static int cycle_count;	// random sz�m gener�l�s�hoz haszn�lt sz�ml�l�
static ship ships[NUMBER_OF_LOCATIONS / 2];	// haj�k helyzete �s �llapota
static gameState state;	// j�t�k jelenlegi �llapota

// j�t�k el�k�sz�t�se
void initializeGame() {
	cycle_count = 0;
	selected = 0;
	shots = 0;
	parts_left = NUMBER_OF_SHIPS_PER_GAME * 2;

	clearDisplay();
	displayStartText();

	state = init;
}

// j�t�k el�k�sz�t�s�nek folytat�sa, miut�n megvan a random seed
void newGame() {
	// haj�k t�mbj�nek t�rl�se
	for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
		ships[i].exists = false;
		ships[i].destroyed[0] = false;
		ships[i].destroyed[1] = false;
	}
	// haj�k elhelyez�se v�letlenszer�en
	srand(cycle_count);

	const int max_step = NUMBER_OF_LOCATIONS / (2 * NUMBER_OF_SHIPS_PER_GAME) - 1; // a legnagyobb l�p�s 2 haj� k�z�tt akkora legyen, hogy �gy is elf�rjenek a kijelz�n
	int location = rand() % max_step; // az els� haj� hely�nek kijel�l�se
	ships[location].exists = true;
	for (int i = 1; i < NUMBER_OF_SHIPS_PER_GAME; ++i) {
		location += (rand() % (max_step - 4)) + 4; // legfeljebb max_step, de legal�bb egy karakternyi hely legyen a haj�k k�z�tt
		ships[location].exists = true;
	}

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

void fireTorpedo(int location) {
	bool will_be_hit = ships[location / 2].exists && ships[location / 2].destroyed[location % 2] == false;
	shots = shots + 1;
	displayShots(shots);
	animShot();
	if (will_be_hit) {
		ships[location / 2].destroyed[location % 2] = true;
		--parts_left;
		animHit();
		displayShip(location / 2, ships[location / 2].destroyed[0], ships[location / 2].destroyed[0]);
		checkWin();
	}
}

// f� loop, �llapotg�p
void gameManager() {
	static bool blink_state;
	int input;			// UART-on �rkezett karakter
	bool segments[2];	// villogtatand� szegmensek

	input = receiveCharacter();

	switch (state) {
		case init:
				if (input != -1) {
					newGame();
					input = -1;
				} else {
					++cycle_count; // random sz�m gener�l�shoz
				}
			break;
		case select:
				blink_state = !blink_state;
				for (int i = 0; i < 2; ++i) { // az egyik szegmens villog, a m�sik az aktu�lis �llapot�t mutatja
					segments[i] = selected % 2 == i ? blink_state : ships[selected / 2].destroyed[i];
				}
				displayShip(selected / 2, segments[0], segments[1]);
				delay();

				switch (input) {
					case 'a': // a: balra
					case 'd': // d: jobbra
						selectShip(input == 'a' ? -1 : 1);
						break;
					case ' ': // space: l�v�s
						fireTorpedo(selected);
						break;
					case 'g': // g: haj�k helyzet�nek kijelz�se
						state = debug;
						break;
					case 'r': // r: j�t�k �jraind�t�sa
						state = init;
						break;
					default:
						break;
				}
			break;
		case debug:
				for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
					displayShip(i, ships[i].exists, ships[i].exists); // �sszes haj� helyzet�nek mutat�sa
				}
				if (input == 'g') {
					for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
						displayShip(i, ships[i].destroyed[0], ships[i].destroyed[1]); // visszat�r�s a j�t�khoz, aktu�lis �llapot kijelz�se
					}
					state = select;
				}
			break;
		default:
			break;
	}
}
