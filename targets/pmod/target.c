#include "target.h"

#include <port.h>

const uint8_t row_pins[NUM_OF_ROWS] =
{
	PIN_PA23, // KBD_ROW1
	PIN_PA25, // KBD_ROW2
	PIN_PA02, // KBD_ROW3
	PIN_PA04, // KBD_ROW4
	PIN_PA05, // KBD_ROW5
	PIN_PA06, // KBD_ROW6
	PIN_PA07, // KBD_ROW7
};

const uint8_t col_pins[NUM_OF_COLS] =
{
	PIN_PA24, // KBD_COL1
	PIN_PA27, // KBD_COL2
	PIN_PA00, // KBD_COL3
	PIN_PA01, // KBD_COL4
	PIN_PA03, // KBD_COL5
};

const uint8_t int_pin = PIN_PA10;

const uint8_t kbd_bl_pin = PIN_PA11;
