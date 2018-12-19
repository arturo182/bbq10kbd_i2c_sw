#include "backlight.h"
#include "conf_target.h"
#include "reg.h"

#include <tc.h>

static struct tc_module instance;

void backlight_update(void)
{
	tc_set_compare_value(&instance, PWM_CHANNEL, reg_get_value(REG_ID_BKL) * 0xFF);
}

void backlight_init(void)
{
	struct tc_config config;
	tc_get_config_defaults(&config);

	config.counter_size    = TC_COUNTER_SIZE_16BIT;
	config.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	config.counter_16_bit.compare_capture_channel[PWM_CHANNEL] = reg_get_value(REG_ID_BKL) * 0xFF;

	config.pwm_channel[PWM_CHANNEL].enabled = true;
	config.pwm_channel[PWM_CHANNEL].pin_out = PWM_OUT_PIN;
	config.pwm_channel[PWM_CHANNEL].pin_mux = PWM_OUT_MUX;

	while (tc_init(&instance, PWM_MODULE, &config) != STATUS_OK) { }
	tc_enable(&instance);
}
