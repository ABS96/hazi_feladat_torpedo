#include "hardware_control.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_device.h"
#include "em_gpio.h"
#include "em_usart.h"
#include "segmentlcd.h"
#include "segmentlcd_individual.h"

// alsó szegmensek értékének tárolására szolgáló struct
static SegmentLCD_LowerCharSegments_TypeDef lowerCharSegments[SEGMENT_LCD_NUM_OF_LOWER_CHARS];

// UART konfigurálása a 4. gyakorlat alapján
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

// nem blokkoló karakter fogadás
int USART_RxNonblocking(USART_TypeDef *usart) {
	if (usart->STATUS & USART_STATUS_RXDATAV) { // ha az UART jelezte a státusz regiszterében, hogy jött adat,
		return (int)(usart->RXDATA);			// akkor azt továbbítjuk
	} else {
		return -1;	// különben -1-gyel jelezzük, hogy nem jött új adat
	}
}

// board, LCD, és UART inicializálása
void initializeHardware() {
	CHIP_Init();
	SegmentLCD_Init(false);
	uartInit();
}

// egyszerû szoftveres késleltetés az animációkhoz
void delay() {
   for(int d = 0; d < DELAY_LENGTH; d++);
}

// kijelzõ törlése
void clearDisplay() {
	displayShots(0);
	for (uint8_t p = 0; p < SEGMENT_LCD_NUM_OF_LOWER_CHARS; p++) {
	  lowerCharSegments[p].raw = 0;
	  SegmentLCD_LowerSegments(lowerCharSegments);
	}
}

// kívánt állapot megjelenítése az adott "hajón"
void displayShip(int location, bool state0, bool state1) {
	const uint16_t map[] = {5, 4, 7, 11, 13, 9, 1, 2}; // hajó helyzetének leképezése egy karakteren belül

	int shiftBy[2] = {
		map[(location % CONFIGURATIONS_PER_CHAR) * 2],
		map[((location % CONFIGURATIONS_PER_CHAR) * 2) + 1]
	}; // szegmensek kiválasztása

	int charNumber = location / CONFIGURATIONS_PER_CHAR; // karakter kiválasztása

	lowerCharSegments[charNumber].raw &= ~0 & ~(1 << shiftBy[0] | 1 << shiftBy[1]);		// aktuális helyen lévõ szegmensek törlése maszkolással
	lowerCharSegments[charNumber].raw |= (state0 << shiftBy[0] | state1 << shiftBy[1]);	// szegmensek bekapcsolása, amennyiben van rajtuk lelõtt hajó

	SegmentLCD_LowerSegments(lowerCharSegments); // alsó kijelzõ frissítése
}

// lövések számának kijelzése
void displayShots(int shots) {
	uint32_t output = 0;
	// átalakítás BCD-be, hogy meg lehessen jeleníteni
    for (int i = 0; i < 4; ++i) {
        output |= (shots % 10) << (4 * i);	// vesszük az aktuális legkisebb helyiértéket
        shots /= 10;						// leosztunk 10-zel, így az egyel nagyobb helyiérték lesz a legkisebb
    }
	SegmentLCD_UnsignedHex(output); // felsõ kijelzõ frissítése
}

// debug függvény UART teszteléshez
void echo() {
	USART_Tx(UART0, USART_Rx(UART0));
}

// wrapper a karakter elvételéhez
int receiveCharacter() {
	return USART_RxNonblocking(UART0);
}

// új játék kezdetekor szöveg kiírása
void displayStartText() {
	SegmentLCD_Write("Ready");
}

// Animációk

void animShot() {
	for(int i = 0; i < 8; ++i) {
		SegmentLCD_ARing(i, 1);   // A-Ring szegmensek bekapcsolása sorban
		delay();
	}

	for(int i = 0; i < 8; ++i) {  // A-Ring szegmensek kikapcsolása
		SegmentLCD_ARing(i, 0);   // ha nincs találat, marad is kikapcsolva
	}
}

void animHit() {                  // találat esetén az összes szegmens felvillan 5-ször
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 8; ++j) {   // A0 .. A7 egyszerre bekapcsol
			SegmentLCD_ARing(j, 1);
		}
		delay();
		for(int k = 0; k < 8; ++k) {  // A0 .. A7 egyszerre kikapcsol
			SegmentLCD_ARing(k, 0);
		}
		delay();
	}
}

void animWin() {
	for(int i = 0; i < 4; ++i) {
		SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 1);
		delay();
		SegmentLCD_Symbol(LCD_SYMBOL_GECKO, 0);
	    delay();
	}
}
