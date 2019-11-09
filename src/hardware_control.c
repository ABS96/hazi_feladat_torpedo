//#pragma once

#include "hardware_control.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"

static SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

void uartInit() {
	// Enable clock for GPIO
	CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_GPIO;

	// Set PF7 to high
	//void GPIO_PinModeSet(GPIO_Port_TypeDef port, unsigned int pin, GPIO_Mode_TypeDef mode, unsigned int out);
	GPIO_PinModeSet(gpioPortF, 7, gpioModePushPull, 1);

	// Configure UART0
	// (Now use the "emlib" functions whenever possible.)

	// Enable clock for UART0
	//void CMU_ClockEnable(CMU_Clock_TypeDef clock, bool enable);
	CMU_ClockEnable(cmuClock_UART0, true);


	// Initialize UART0 (115200 Baud, 8N1 frame format)

	// To initialize the UART0, we need a structure to hold
	// configuration data. It is a good practice to initialize it with
	// default values, then set individual parameters only where needed.
	USART_InitAsync_TypeDef UART0_init = USART_INITASYNC_DEFAULT;

	//void USART_InitAsync(USART_TypeDef *usart, const USART_InitAsync_TypeDef *init);
	USART_InitAsync(UART0, &UART0_init);
	// USART0: see in efm32ggf1024.h

	// Set TX (PE0) and RX (PE1) pins as push-pull output and input resp.
	// DOUT for TX is 1, as it is the idle state for UART communication
	GPIO_PinModeSet(gpioPortE, 0, gpioModePushPull, 1);
	// DOUT for RX is 0, as DOUT can enable a glitch filter for inputs,
	// and we are fine without such a filter
	GPIO_PinModeSet(gpioPortE, 1, gpioModeInput, 0);

	// Use PE0 as TX and PE1 as RX (Location 1, see datasheet (not refman))
		// Enable both RX and TX for routing
	UART0->ROUTE |= UART_ROUTE_LOCATION_LOC1;
		// Select "Location 1" as the routing configuration
	UART0->ROUTE |= UART_ROUTE_TXPEN | UART_ROUTE_RXPEN;
}

int USART_RxNonblocking(USART_TypeDef *usart) {
	if (usart->STATUS & USART_STATUS_RXDATAV) {
		return (int)(usart->RXDATA);
	} else {
		return -1;
	}
}

void initializeHardware() {
	CHIP_Init();
	SegmentLCD_Init(false);
	uartInit();
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
	const uint16_t map[] = {5, 4, 7, 11, 13, 9, 1, 2}; // hajó helyzetének leképezése szegmensekre

	int shiftBy[2] = {
		map[(location % CONFIGURATIONS_PER_CHAR) * 2],
		map[((location % CONFIGURATIONS_PER_CHAR) * 2) + 1]
	};

	int charNumber = location / CONFIGURATIONS_PER_CHAR;

	lowerCharSegments[charNumber].raw &= ~0 & ~(1 << shiftBy[0] | 1 << shiftBy[1]);		// aktuális helyen lévõ szegmensek törlése
	lowerCharSegments[charNumber].raw |= (state0 << shiftBy[0] | state1 << shiftBy[1]);	// szegmensek bekapcsolása, amennyiben van rajtuk lelõtt hajó

	SegmentLCD_LowerSegments(lowerCharSegments);
}

void displayShots(int shots) {
	uint32_t output = 0;
	// átalakítás BCD-be
    for (int i = 0; i < 4; ++i) {
        output |= (shots % 10) << (4 * i);
        shots /= 10;
    }
	SegmentLCD_UnsignedHex(output);
}

void echo() {
	USART_Tx(UART0, USART_Rx(UART0));
}

int receiveCharacter() {
	return USART_RxNonblocking(UART0);
}

void transmitCharacter(int character) {
	USART_Tx(UART0, character);
}
