#pragma once

#include "hardware_control.h"
#include "em_device.h"
#include "em_chip.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"

static SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

void initializeHardware() {
	CHIP_Init();
	SegmentLCD_Init(false);
	// #TODO uart
}

void delay() {
   for(int d=0;d<DELAY_LENGTH;d++); // #TODO ez legyen hardveres
}

void clearDisplay() {
   for (uint8_t p = 0; p < SEGMENT_LCD_NUM_OF_LOWER_CHARS; p++) {
	  lowerCharSegments[p].raw = 0;
	  SegmentLCD_LowerSegments(lowerCharSegments);
   }
}


void displayShip(int location, bool state0, bool state1) {
	const uint16_t map[] = {4, 5, 7, 11, 9, 13, 1, 2}; // haj� helyzet�nek lek�pez�se szegmensekre

	static bool blinkToggle = false;

	int shiftBy[2] = {
		map[(location % CONFIGURATIONS_PER_CHAR) * 2],
		map[((location % CONFIGURATIONS_PER_CHAR) * 2) + 1]
	};

	int charNumber = location / CONFIGURATIONS_PER_CHAR;

	lowerCharSegments[charNumber].raw &= ~0 & ~(1 << shiftBy[0] | 1 << shiftBy[1]);		// aktu�lis helyen l�v� szegmensek t�rl�se
	lowerCharSegments[charNumber].raw |= (state0 << shiftBy[0] | state1 << shiftBy[1]);	// szegmensek bekapcsol�sa, amennyiben van rajtuk lel�tt haj�

	SegmentLCD_LowerSegments(lowerCharSegments);
}

void displayShots(int shots) {
	SegmentLCD_UnsignedHex(shots);
}
