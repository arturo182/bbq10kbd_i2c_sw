#include "target.h"

#include <port.h>

const uint8_t row_pins[NUM_OF_ROWS] =
{
	PIN_PB11, // KBD_ROW1
	PIN_PA13, // KBD_ROW2
	PIN_PA17, // KBD_ROW3
	PIN_PA19, // KBD_ROW4
	PIN_PA20, // KBD_ROW5
	PIN_PA21, // KBD_ROW6
	PIN_PA23, // KBD_ROW7
};

const uint8_t col_pins[NUM_OF_COLS] =
{
	PIN_PA12, // KBD_COL1
	PIN_PA14, // KBD_COL2
	PIN_PA15, // KBD_COL3
	PIN_PA16, // KBD_COL4
	PIN_PA18, // KBD_COL5
};

const uint8_t btn_pins[NUM_OF_BTNS] =
{
	PIN_PA07, // JOY_UP
	PIN_PB09, // JOY_DOWN
	PIN_PA05, // JOY_LEFT
	PIN_PA04, // JOY_RIGHT
	PIN_PA06, // JOY_CENTER
	PIN_PA00, // BTN_LEFT1
	PIN_PA03, // BTN_RIGHT1
	PIN_PA01, // BTN_LEFT2
	PIN_PA02, // BTN_RIGHT2
};

const uint8_t int_pin = PIN_PB22;

const uint8_t kbd_bl_pin = PIN_PB10;
