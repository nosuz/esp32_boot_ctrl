TARGET = esp32_boot_ctlr
SRCS = esp32_boot_ctlr.c

MCU=attiny212

# options for udpipy
COM = /dev/ttyUSB0
#SPEED = 230400

AVR_WRITER = updipy -d $(MCU) -l $(COM)
#AVR_WRITER += -b $(SPEED)

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump

MPACKS = /usr/local/MicrochipPacks

DEV_DIR = -B $(MPACKS)/gcc/dev/$(MCU)
INC_DIR = -I $(MPACKS)/include

CFLAGS = -Wall
CFLAGS += -mmcu=$(MCU)
CFLAGS += -Os -g
CFLAGS += $(DEV_DIR)
CFLAGS += $(INC_DIR)

LDFLAGS = -mmcu=$(MCU)
LDFLAGS += $(DEV_DIR)
#LDFLAGS += -Wl,-Map=$(TARGET).map


OBJS = $(SRCS:%.c=%.o)
DEPS = $(SRCS:%.c=%.d)

all: clean $(TARGET).hex

clean:
	rm -f $(OBJS) $(DEPS)
	rm -f $(TARGET).elf $(TARGET).hex
	rm -f $(TARGET).map $(TARGET).s
	rm -f *~

$(TARGET).elf: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TARGET).hex: $(TARGET).elf
	#$(OBJCOPY) -O ihex -j .text $< $@
	$(OBJCOPY) -O ihex $< $@

flash: $(TARGET).hex
	$(AVR_WRITER) -i $(TARGET).hex

dump: $(TARGET).hex
	$(OBJDUMP) -S -D $(TARGET).elf > $(TARGET).s

.c.o:
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)
