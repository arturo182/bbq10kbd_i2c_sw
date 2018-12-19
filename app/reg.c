#include "reg.h"

static uint8_t regs[REG_ID_LAST];

uint8_t reg_get_value(enum reg_id reg)
{
	return regs[reg];
}

void reg_set_value(enum reg_id reg, uint8_t value)
{
	regs[reg] = value;
}

bool reg_is_bit_set(enum reg_id reg, uint8_t bit)
{
	return regs[reg] & bit;
}

void reg_set_bit(enum reg_id reg, uint8_t bit)
{
	regs[reg] |= bit;
}

void reg_init(void)
{
	regs[REG_ID_CFG] = CFG_KEY_INT;
	regs[REG_ID_DEB] = 10;
	regs[REG_ID_FRQ] = 5;
	regs[REG_ID_BKL] = 255;
}
