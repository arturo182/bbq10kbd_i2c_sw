#include <delay/delay.h>
#include <port.h>
#include <usart/usart.h>

#include "backlight.h"
#include "fifo.h"
#include "i2c.h"
#include "reg.h"
#include "target.h"
#include "time.h"

#ifdef DEBUG
static struct usart_module usart_instance;
#endif

#ifdef DEBUG
static void configure_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);

	config_usart.baudrate    = 115200;
	config_usart.mux_setting = CONF_UART_SERCOM_MUX_SETTING;
	config_usart.pinmux_pad0 = CONF_UART_SERCOM_PINMUX_PAD0;
	config_usart.pinmux_pad1 = CONF_UART_SERCOM_PINMUX_PAD1;
	config_usart.pinmux_pad2 = CONF_UART_SERCOM_PINMUX_PAD2;
	config_usart.pinmux_pad3 = CONF_UART_SERCOM_PINMUX_PAD3;

	while (usart_init(&usart_instance, CONF_UART_MODULE, &config_usart) != STATUS_OK) { }
	usart_enable(&usart_instance);
}

int _write(void *fd, const char *msg, size_t len)
{
	usart_write_buffer_wait(&usart_instance, (const uint8_t*)msg, len);

	return len;
}

int _read(void *fd, char *msg, size_t len)
{
	usart_read_buffer_wait(&usart_instance, (uint8_t *)msg, len);

	return len;
}
#endif

static void key_cb(char key, enum key_state state)
{
	bool do_int = false;

	if (reg_is_bit_set(REG_ID_CFG, CFG_KEY_INT)) {
		reg_set_bit(REG_ID_INT, INT_KEY);
		do_int = true;
	}

#ifdef DEBUG
	printf("key: 0x%02X/%d/%c, state: %d, blk: %d\r\n", key, key, key, state, reg_get_value(REG_ID_BKL));
#endif

	const struct fifo_item item = { key, state };
	if (!fifo_enqueue(item)) {
		if (reg_is_bit_set(REG_ID_CFG, CFG_OVERFLOW_INT)) {
			reg_set_bit(REG_ID_INT, INT_OVERFLOW);
			do_int = true;
		}

		// TODO: Should we flush on full?
		fifo_flush();
	}

	if (do_int) {
		port_pin_set_output_level(int_pin, 0);
		delay_ms(INT_DURATION_MS);
		port_pin_set_output_level(int_pin, 1);
	}
}

static void lock_cb(bool caps_changed, bool num_changed)
{
	bool do_int = false;

	if (caps_changed && reg_is_bit_set(REG_ID_CFG, CFG_CAPSLOCK_INT)) {
		reg_set_bit(REG_ID_INT, INT_CAPSLOCK);
		do_int = true;
	}

	if (num_changed && reg_is_bit_set(REG_ID_CFG, CFG_NUMLOCK_INT)) {
		reg_set_bit(REG_ID_INT, INT_NUMLOCK);
		do_int = true;
	}

#if DEBUG
	printf("lock, caps_c: %d, caps: %d, num_c: %d, num: %d\r\n",
		   caps_changed, keyboard_get_capslock(),
		   num_changed, keyboard_get_numlock());
#endif

	if (do_int) {
		port_pin_set_output_level(int_pin, 0);
		delay_ms(INT_DURATION_MS);
		port_pin_set_output_level(int_pin, 1);
	}
}

static void config_int_pin(void)
{
	struct port_config port_init;
	port_get_config_defaults(&port_init);

	port_init.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(int_pin, &port_init);
	port_pin_set_output_level(int_pin, 1);
}

int main(void)
{
	// TODO: system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);

	system_init();
	delay_init();
	time_init();

#ifdef DEBUG
	configure_usart();
#endif

	config_int_pin();
	reg_init();
	backlight_init();

	keyboard_init();
	keyboard_set_key_callback(key_cb);
	keyboard_set_lock_callback(lock_cb);

	i2c_init();

#ifdef DEBUG
	printf("Starting main loop\r\n");
#endif

	while (true) {
		keyboard_process();
	}

	return 0;
}
