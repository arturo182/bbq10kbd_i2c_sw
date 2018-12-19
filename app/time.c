#include "time.h"

#include <clock_feature.h>

static volatile uint32_t uptime_ms;

void SysTick_Handler(void)
{
	++uptime_ms;
}

void time_delay_ms(uint32_t delay)
{
	const uint32_t start_ms = uptime_ms;

	while((uptime_ms - start_ms) < delay)
		;
}

uint32_t time_uptime_ms(void)
{
	return uptime_ms;
}

void time_init(void)
{
	SysTick_Config(system_cpu_clock_get_hz() / 1000);
}
