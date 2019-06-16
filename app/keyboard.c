#include "conf_app.h"
#include "keyboard.h"
#include "reg.h"
#include "target.h"
#include "time.h"
#include "util.h"

#include <stdlib.h>
#include <port.h>

enum mod
{
	MOD_NONE = 0,
	MOD_SYM,
	MOD_ALT,
	MOD_SHL,
	MOD_SHR,

	MOD_LAST,
};

struct entry
{
	char chr;
	char symb;
	enum mod mod;
};

struct list_item
{
	const struct entry *p_entry;
	uint32_t hold_start_time;
	enum key_state state;
	bool mods[MOD_LAST];
};

static const struct entry kbd_entries[][NUM_OF_COLS] =
{
	{ { 'Q', '#' },       { 'E', '2' },       { 'R', '3' },       { 'U', '_'  }, { 'O', '+'  } },
	{ { 'W', '1' },       { 'S', '4' },       { 'G', '/' },       { 'H', ':'  }, { 'L', '"'  } },
	{ { .mod = MOD_SYM }, { 'D', '5' },       { 'T', '(' },       { 'Y', ')'  }, { 'I', '-'  } },
	{ { 'A', '*' },       { 'P', '@' },       { .mod = MOD_SHR }, { '\n', '|' }, { '\b'      } },
	{ { .mod = MOD_ALT }, { 'X', '8' },       { 'V', '?' },       { 'B', '!'  }, { '$', '`'  } },
	{ { ' ', '\t' },      { 'Z', '7' },       { 'C', '9' },       { 'N', ','  }, { 'M', '.'  } },
	{ { '~', '0' },       { .mod = MOD_SHL }, { 'F', '6' },       { 'J', ';'  }, { 'K', '\'' } },
};

#if NUM_OF_BTNS > 0
static const struct entry btn_entries[NUM_OF_BTNS] =
{
	{ KEY_JOY_UP     },
	{ KEY_JOY_DOWN   },
	{ KEY_JOY_LEFT   },
	{ KEY_JOY_RIGHT  },
	{ KEY_JOY_CENTER },
	{ KEY_BTN_LEFT1  },
	{ KEY_BTN_RIGHT1 },
#if NUM_OF_BTNS > 7
	{ KEY_BTN_LEFT2  },
	{ KEY_BTN_RIGHT2 },
#endif
};
#endif

static struct {
	lock_callback lock_callback;
	key_callback key_callback;

	struct list_item list[KEY_LIST_SIZE];

	uint32_t last_process_time;

	bool mods[MOD_LAST];

	bool capslock_changed;
	bool capslock;

	bool numlock_changed;
	bool numlock;
} self;

static void transition_to(struct list_item * const p_item, const enum key_state next_state)
{
	const struct entry * const p_entry = p_item->p_entry;

	p_item->state = next_state;

	if (!self.key_callback || !p_entry)
		return;

	char chr = p_entry->chr;

	switch (p_entry->mod) {
		case MOD_ALT:
			if (reg_is_bit_set(REG_ID_CFG, CFG_REPORT_MODS))
				chr = 17;
			break;

		case MOD_SHL:
		case MOD_SHR:
			if (reg_is_bit_set(REG_ID_CFG, CFG_REPORT_MODS))
				chr = 18;
			break;

		case MOD_SYM:
			if (reg_is_bit_set(REG_ID_CFG, CFG_REPORT_MODS))
				chr = 19;
			break;

		default:
		{
			if (reg_is_bit_set(REG_ID_CFG, CFG_USE_MODS)) {
				const bool shift = (self.mods[MOD_SHL] || self.mods[MOD_SHR]) | self.capslock;
				const bool alt = self.mods[MOD_ALT] | self.numlock;

				if (alt) {
					chr = p_entry->symb;
				} else if (!shift && (chr >= 'A' && chr <= 'Z')) {
					chr = (chr + ' ');
				}
			}

			break;
		}
	}

	if (chr != 0)
		self.key_callback(chr, next_state);
}

static void next_item_state(struct list_item * const p_item, const bool pressed)
{
	switch (p_item->state) {
		case KEY_STATE_IDLE:
			if (pressed) {
				if (p_item->p_entry->mod != MOD_NONE)
					self.mods[p_item->p_entry->mod] = true;

				if (!self.capslock_changed && self.mods[MOD_SHR] && self.mods[MOD_ALT]) {
					self.capslock = true;
					self.capslock_changed = true;
				}

				if (!self.numlock_changed && self.mods[MOD_SHL] && self.mods[MOD_ALT]) {
					self.numlock = true;
					self.numlock_changed = true;
				}

				if (!self.capslock_changed && (self.mods[MOD_SHL] || self.mods[MOD_SHR])) {
					self.capslock = false;
					self.capslock_changed = true;
				}

				if (!self.numlock_changed && (self.mods[MOD_SHL] || self.mods[MOD_SHR])) {
					self.numlock = false;
					self.numlock_changed = true;
				}

				if (!self.mods[MOD_ALT]) {
					self.capslock_changed = false;
					self.numlock_changed = false;
				}

				if (self.lock_callback && (self.capslock_changed || self.numlock_changed))
					self.lock_callback(self.capslock_changed, self.numlock_changed);

				transition_to(p_item, KEY_STATE_PRESSED);

				p_item->hold_start_time = time_uptime_ms();
			}
			break;

		case KEY_STATE_PRESSED:
			if ((time_uptime_ms() - p_item->hold_start_time) > KEY_HOLD_TIME) {
				transition_to(p_item, KEY_STATE_HOLD);
			 } else if(!pressed) {
				transition_to(p_item, KEY_STATE_RELEASED);
			}
			break;

		case KEY_STATE_HOLD:
			if (!pressed)
				transition_to(p_item, KEY_STATE_RELEASED);
			break;
		case KEY_STATE_RELEASED:
		{
			if (p_item->p_entry->mod != MOD_NONE)
				self.mods[p_item->p_entry->mod] = false;

			p_item->p_entry = NULL;
			transition_to(p_item, KEY_STATE_IDLE);
			break;
		}
	}
}

void keyboard_process(void)
{
	struct port_config port_init;

	if ((time_uptime_ms() - self.last_process_time) <= KEY_POLL_TIME)
		return;

	port_get_config_defaults(&port_init);

	for (uint32_t c = 0; c < NUM_OF_COLS; ++c) {
		port_init.direction = PORT_PIN_DIR_OUTPUT;
		port_pin_set_config(col_pins[c], &port_init);
		port_pin_set_output_level(col_pins[c], 0);

		for (uint32_t r = 0; r < NUM_OF_ROWS; ++r) {
			const bool pressed = (port_pin_get_input_level(row_pins[r]) == 0);
			const int32_t key_idx = (int32_t)((r * NUM_OF_COLS) + c);

			int32_t list_idx = -1;
			for (int32_t i = 0; i < KEY_LIST_SIZE; ++i) {
				if (self.list[i].p_entry != &((const struct entry*)kbd_entries)[key_idx])
					continue;

				list_idx = i;
				break;
			}

			if (list_idx > -1) {
				next_item_state(&self.list[list_idx], pressed);
				continue;
			}

			if (!pressed)
				continue;

			for (uint32_t i = 0 ; i < KEY_LIST_SIZE; ++i) {
				if (self.list[i].p_entry != NULL)
					continue;

				self.list[i].p_entry = &((const struct entry*)kbd_entries)[key_idx];
				self.list[i].state = KEY_STATE_IDLE;
				next_item_state(&self.list[i], pressed);

				break;
			}
		}

		port_pin_set_output_level(col_pins[c], 1);

		port_init.direction = PORT_PIN_DIR_INPUT;
		port_init.input_pull = PORT_PIN_PULL_NONE;
		port_pin_set_config(col_pins[c], &port_init);
	}

#if NUM_OF_BTNS > 0
	for (uint32_t b = 0; b < NUM_OF_BTNS; ++b) {
		const bool pressed = (port_pin_get_input_level(btn_pins[b]) == 0);

		int32_t list_idx = -1;
		for (int32_t i = 0; i < KEY_LIST_SIZE; ++i) {
			if (self.list[i].p_entry != &((const struct entry*)btn_entries)[b])
				continue;

			list_idx = i;
			break;
		}

		if (list_idx > -1) {
			next_item_state(&self.list[list_idx], pressed);
			continue;
		}

		if (!pressed)
			continue;

		for (uint32_t i = 0 ; i < KEY_LIST_SIZE; ++i) {
			if (self.list[i].p_entry != NULL)
				continue;

			self.list[i].p_entry = &((const struct entry*)btn_entries)[b];
			self.list[i].state = KEY_STATE_IDLE;
			next_item_state(&self.list[i], pressed);

			break;
		}
	}
#endif

	self.last_process_time = time_uptime_ms();
}

void keyboard_set_key_callback(key_callback callback)
{
	self.key_callback = callback;
}

void keyboard_set_lock_callback(lock_callback callback)
{
	self.lock_callback = callback;
}

bool keyboard_get_capslock(void)
{
	return self.capslock;
}

bool keyboard_get_numlock(void)
{
	return self.numlock;
}

void keyboard_init(void)
{
	struct port_config port_init;
	port_get_config_defaults(&port_init);

	for (int i = 0; i < MOD_LAST; ++i)
		self.mods[i] = false;

	// Rows
	port_init.direction = PORT_PIN_DIR_INPUT;
	port_init.input_pull = PORT_PIN_PULL_UP;
	for (uint32_t i = 0; i < NUM_OF_ROWS; ++i)
		port_pin_set_config(row_pins[i], &port_init);

	// Cols
	port_init.direction = PORT_PIN_DIR_INPUT;
	port_init.input_pull = PORT_PIN_PULL_NONE;
	for(uint32_t i = 0; i < NUM_OF_COLS; ++i)
		port_pin_set_config(col_pins[i], &port_init);

	// Btns
#if NUM_OF_BTNS > 0
	port_init.direction = PORT_PIN_DIR_INPUT;
	port_init.input_pull = PORT_PIN_PULL_UP;
	for(uint32_t i = 0; i < NUM_OF_BTNS; ++i)
		port_pin_set_config(btn_pins[i], &port_init);
#endif
}
