include $(PLATFORM)/common.mk
include $(PLATFORM)/local.mk

TOOLS ?= $(WINAVR-PATH)/bin
GCCARCH = avr
MCU = atmega328p

COPTS = -mmcu=$(MCU) -I$(WINAVR-PATH)/avr/include
LFLAGS = -mmcu=$(MCU) --entry __init -u __init -nostdlib -Wl,-Map=$(OUTDIR)/$(MAIN).map,--gc-sections

EXEC = $(PLATFORM)/Avr/avrdude -C $(PLATFORM)/Avr/avrdude.conf -c stk500v1 -p $(MCU) -P $(COM-PORT) -q -q -V -F -U flash:w:$(HEXFILE)

$(OUTFILE): $(OBJECTS)
	$(CC) $(LFLAGS) -o $@ $(PLATFORM)/Avr/crt0.o $^ -T $(PLATFORM)/Avr/lnk.cmd
	$(OBJCOPY) -O ihex $(OUTFILE) $(HEXFILE)
	$(SIZE) $@
