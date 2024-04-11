DEVICE = msp430g2553

FLASH = mspdebug rf2500
CC    = msp430-elf-gcc

CFLAGS  = -I/usr/msp430-elf/include -mmcu=$(DEVICE) -O2 -g
LDFLAGS = -L/usr/msp430-elf/lib/430 -T /usr/msp430-elf/lib/$(DEVICE).ld

OBJECTS = main.o
TARGET  = flash.elf

all: ${OBJECTS}
	$(CC) $(CFLAGS) $(LDFLAGS) $? -o $(TARGET)

clean:
	rm -rf $(TARGET) *.o

flash: all
	$(FLASH) "prog $(TARGET)"

run: flash
	sleep 3
	$(FLASH) run
