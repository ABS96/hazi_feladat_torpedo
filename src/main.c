#include "game_logic.h"
#include "hardware_control.h"

int main(void)
{
	initializeHardware();
	initializeGame();

  while (1) {
	  gameManager();
  }
}
