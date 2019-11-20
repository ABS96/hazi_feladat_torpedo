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

static int shots;		// eddigi lövések száma
static int parts_left;	// még elpusztítandó szegmensek száma
static int selected;	// éppen aktív hajó
static int cycle_count;	// random szám generálásához használt számláló
static ship ships[NUMBER_OF_LOCATIONS / 2];	// hajók helyzete és állapota
static gameState state;	// játék jelenlegi állapota

// játék elõkészítése
void initializeGame() {
	cycle_count = 0;
	selected = 0;
	shots = 0;
	parts_left = NUMBER_OF_SHIPS_PER_GAME * 2;

	clearDisplay();
	displayStartText();

	state = init;
}

// játék elõkészítésének folytatása, miután megvan a random seed
void newGame() {
	// hajók tömbjének törlése
	for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
		ships[i].exists = false;
		ships[i].destroyed[0] = false;
		ships[i].destroyed[1] = false;
	}
	// hajók elhelyezése véletlenszerûen
	srand(cycle_count);

	const int max_step = NUMBER_OF_LOCATIONS / (2 * NUMBER_OF_SHIPS_PER_GAME) - 1; // a legnagyobb lépés 2 hajó között akkora legyen, hogy így is elférjenek a kijelzõn
	int location = rand() % max_step; // az elsõ hajó helyének kijelölése
	ships[location].exists = true;
	for (int i = 1; i < NUMBER_OF_SHIPS_PER_GAME; ++i) {
		location += (rand() % (max_step - 4)) + 4; // legfeljebb max_step, de legalább egy karakternyi hely legyen a hajók között
		ships[location].exists = true;
	}

	state = select;
}

void selectShip(int dir) {
	// Legutóbb villogtatott hajó visszaállítása
	displayShip(selected / 2, ships[selected / 2].destroyed[0], ships[selected / 2].destroyed[1]);

	// Léptetés a tömbön
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

// fõ loop, állapotgép
void gameManager() {
	static bool blink_state;
	int input;			// UART-on érkezett karakter
	bool segments[2];	// villogtatandó szegmensek

	input = receiveCharacter();

	switch (state) {
		case init:
				if (input != -1) {
					newGame();
					input = -1;
				} else {
					++cycle_count; // random szám generáláshoz
				}
			break;
		case select:
				blink_state = !blink_state;
				for (int i = 0; i < 2; ++i) { // az egyik szegmens villog, a másik az aktuális állapotát mutatja
					segments[i] = selected % 2 == i ? blink_state : ships[selected / 2].destroyed[i];
				}
				displayShip(selected / 2, segments[0], segments[1]);
				delay();

				switch (input) {
					case 'a': // a: balra
					case 'd': // d: jobbra
						selectShip(input == 'a' ? -1 : 1);
						break;
					case ' ': // space: lövés
						fireTorpedo(selected);
						break;
					case 'g': // g: hajók helyzetének kijelzése
						state = debug;
						break;
					case 'r': // r: játék újraindítása
						state = init;
						break;
					default:
						break;
				}
			break;
		case debug:
				for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
					displayShip(i, ships[i].exists, ships[i].exists); // összes hajó helyzetének mutatása
				}
				if (input == 'g') {
					for (int i = 0; i < NUMBER_OF_LOCATIONS / 2; ++i) {
						displayShip(i, ships[i].destroyed[0], ships[i].destroyed[1]); // visszatérés a játékhoz, aktuális állapot kijelzése
					}
					state = select;
				}
			break;
		default:
			break;
	}
}
