#pragma once

#include <stdbool.h>

#define CONFIGURATIONS_PER_CHAR 4
#define DELAY_LENGTH 300000

void initializeHardware();
void clearDisplay();
void delay();
int receiveCharacter();
void transmitCharacter(int character);
void displayShip(int location, bool state0, bool state1);
void displayShots(int shots);
