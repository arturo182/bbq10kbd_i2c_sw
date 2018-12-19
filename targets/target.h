#pragma once

#include "conf_target.h"

#include <stdint.h>

extern const uint8_t row_pins[NUM_OF_ROWS];
extern const uint8_t col_pins[NUM_OF_COLS];

#if NUM_OF_BTNS > 0
extern const uint8_t btn_pins[NUM_OF_BTNS];
#endif

extern const uint8_t int_pin;
extern const uint8_t kbd_bl_pin;
