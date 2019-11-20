#pragma once

#include <stdbool.h>

#define CONFIGURATIONS_PER_CHAR 4
#define DELAY_LENGTH 140000

void initializeHardware();
void clearDisplay();
void delay();
int receiveCharacter();
void displayStartText();
void displayShip(int location, bool state0, bool state1);
void displayShots(int shots);
void animShot();
void animHit();
void animWin();
