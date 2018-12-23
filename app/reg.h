#pragma once

#include "conf_app.h"

#include <stdbool.h>
#include <stdint.h>

enum reg_id
{
	REG_ID_VER = 0x01, // fw version
	REG_ID_CFG = 0x02, // config
	REG_ID_INT = 0x03, // interrupt status
	REG_ID_KEY = 0x04, // key status
	REG_ID_BKL = 0x05, // backlight
	REG_ID_DEB = 0x06, // debounce cfg
	REG_ID_FRQ = 0x07, // poll freq cfg
	REG_ID_RST = 0x08, // reset
	REG_ID_FIF = 0x09, // fifo

	REG_ID_LAST,
};

#define CFG_OVERFLOW_ON		(1 << 0)
#define CFG_OVERFLOW_INT	(1 << 1)
#define CFG_CAPSLOCK_INT	(1 << 2)
#define CFG_NUMLOCK_INT		(1 << 3)
#define CFG_KEY_INT			(1 << 4)
#define CFG_PANIC_INT		(1 << 5)
#define CFG_REPORT_MODS		(1 << 6) // Should Alt, Sym and Shifts be reported as well

#define INT_OVERFLOW		(1 << 0)
#define INT_CAPSLOCK		(1 << 1)
#define INT_NUMLOCK			(1 << 2)
#define INT_KEY				(1 << 3)
#define INT_PANIC			(1 << 4)

#define KEY_CAPSLOCK		(1 << 5)
#define KEY_NUMLOCK			(1 << 6)
#define KEY_COUNT_MASK		0x1F

#define VER_VAL				((VERSION_MAJOR << 4) | (VERSION_MINOR << 0))

uint8_t reg_get_value(enum reg_id reg);
void reg_set_value(enum reg_id reg, uint8_t value);
bool reg_is_bit_set(enum reg_id reg, uint8_t bit);
void reg_set_bit(enum reg_id reg, uint8_t bit);
void reg_init(void);
