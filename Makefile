CROSS_COMPILE ?= arm-none-eabi-
TARGET        ?= lagom

CC      := $(CROSS_COMPILE)gcc
LD      := $(CROSS_COMPILE)ld
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

SRCS := \
	SDK/common/utils/interrupt/interrupt_sam_nvic.c \
	SDK/common2/services/delay/sam0/cycle_counter.c \
	SDK/sam0/drivers/port/port.c \
	SDK/sam0/drivers/sercom/sercom.c \
	SDK/sam0/drivers/sercom/sercom_interrupt.c \
	SDK/sam0/drivers/sercom/usart/usart.c \
	SDK/sam0/drivers/system/clock/clock_samd20/clock.c \
	SDK/sam0/drivers/system/clock/clock_samd20/gclk.c \
	SDK/sam0/drivers/sercom/i2c/i2c_samd20/i2c_slave.c \
	SDK/sam0/drivers/sercom/i2c/i2c_samd20/i2c_slave_interrupt.c \
	SDK/sam0/drivers/system/pinmux/pinmux.c \
	SDK/sam0/drivers/system/system.c \
	SDK/sam0/drivers/tc/tc_sam_d_r_h/tc.c \
	SDK/sam0/utils/cmsis/samd20/source/gcc/startup_samd20.c \
	SDK/sam0/utils/cmsis/samd20/source/system_samd20.c \
	SDK/sam0/utils/syscalls/gcc/syscalls.c \
	targets/$(TARGET)/target.c \
	app/backlight.c \
	app/fifo.c \
	app/i2c.c \
	app/keyboard.c \
	app/main.c \
	app/reg.c \
	app/time.c \

INCS := \
	SDK/common/utils \
	SDK/common2/services \
	SDK/common2/services/delay \
	SDK/sam0/drivers/gpio \
	SDK/sam0/drivers/port \
	SDK/sam0/drivers/sercom \
	SDK/sam0/drivers/sercom/i2c \
	SDK/sam0/drivers/system \
	SDK/sam0/drivers/system/clock \
	SDK/sam0/drivers/system/clock/clock_samd20 \
	SDK/sam0/drivers/sercom \
	SDK/sam0/drivers/system/interrupt \
	SDK/sam0/drivers/system/interrupt/system_interrupt_samd20 \
	SDK/sam0/drivers/system/pinmux \
	SDK/sam0/drivers/system/power/power_sam_d_r_h \
	SDK/sam0/drivers/system/reset/reset_sam_d_r_h \
	SDK/sam0/drivers/tc \
	SDK/sam0/utils \
	SDK/sam0/utils/cmsis/samd20/include \
	SDK/sam0/utils/cmsis/samd20/source \
	SDK/sam0/utils/header_files \
	SDK/sam0/utils/preprocessor \
	SDK/thirdparty/CMSIS/Include \
	targets/$(TARGET) \
	targets \
	app/config \
	app

DEFS := \
	I2C_SLAVE_CALLBACK_MODE=true \
	__SAMD20E16__ \
	CYCLE_MODE

LIBS := \

LDSCRIPT := SDK/sam0/utils/linker_scripts/samd20/gcc/samd20e16_flash.ld

OBJS := $(patsubst %.c,out/$(TARGET)/obj/%.c.o, $(filter %.c, $(SRCS)))
DEPS := $(patsubst %.o,%.d,$(OBJS))

CFLAGS := \
	-mcpu=cortex-m0plus -mthumb \
	-std=gnu99 -Os -g -Wall -Wextra \
	-Wno-missing-field-initializers -Wno-unused-parameter -Wno-expansion-to-defined \
	-ffunction-sections -fdata-sections -fomit-frame-pointer \
	$(addprefix -D, $(DEFS)) \
	$(addprefix -I, $(INCS))

LDFLAGS := \
	-mcpu=cortex-m0plus -mthumb \
	-Wl,--gc-sections --specs=nano.specs \
	-T$(LDSCRIPT) \
	$(LIBS)

all: out/$(TARGET)/app.bin
.PHONY: all

out/$(TARGET)/obj/%.c.o: %.c
	@echo "CC $<"
	@mkdir -p $(dir $@)
	@$(CC) -MM -MF $(subst .o,.d,$@) -MP -MT "$@ $(subst .o,.d,$@)" $(CFLAGS) $<
	@$(CC) $(CFLAGS) -c -o $@ $<

out/$(TARGET)/app.elf: $(OBJS)
	@echo "LD $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -Xlinker -Map=$@.map
	@$(SIZE) $@

%.bin: %.elf
	@$(OBJCOPY) -O binary $< $@

clean:
	@echo "Removing the out directory"
	@rm -rf out/
.PHONY: clean

flash: out/$(TARGET)/app.bin
	@echo -e "reset halt\nprogram $< verify\nreset run\nexit\n" | nc localhost 4444
.PHONY: flash

daplink:
	@openocd \
	-f interface/cmsis-dap.cfg \
	-c "transport select swd" \
	-f target/at91samdXX.cfg \
	-c "init ; reset halt"
.PHONY: daplink

jlink:
	@openocd \
	-f interface/jlink.cfg \
	-c "transport select swd" \
	-c "set WORKAREASIZE 0" \
	-f target/at91samdXX.cfg \
	-c "init ; reset halt"
.PHONY: jlink

ifneq ("$(MAKECMDGOALS)","clean")
-include $(DEPS)
endif
