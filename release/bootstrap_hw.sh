#!/bin/bash

# 1. variablen initialisieren
SERIAL_PORT=/dev/ttyUSB0
AVRDUDE=avrdude
AVRDUDE_PROGRAMMER=dragon_jtag
AVRDUDE_PORT=usb
AVRDUDE_MCU=atxmega192a3
AVRDUDE_OPT="-v -v"

AVRDUDE_PROGRAMMER_XBOOTLOADER=avr911
BAUDRATE_XBOOTLOADER=115200

AVRDUDE_WRITE_XBOOTLOADER=flash:w:Xmega_Bootloader.hex:i
AVRDUDE_WRITE_FIRMWARE=flash:w:AmbiController.hex:i
AVRDUDE_WRITE_EEPROM=flash:w:eeprom.hex:i

BCTOOL=./bctool
BCTOOL_OPT=-u
CHANNEL_CONFIG_FILE=channel_config_16_24_16_8_8.cfg

FPGA_FLASH=./fpga_flash
FPGA_FLASH_OPT=-s
FPGA_FLASH_IMG=main_20x02_EP4CE6.img

RTBT=./rtbt
POST_RTBT_SLEEP_TIME=4

# 2. write xbootloader
$(AVRDUDE) $(AVRDUDE_OPT) -P $(AVRDUDE_PORT) -p $(AVRDUDE_MCU) -c $(AVRDUDE_PROGRAMMER) -U $(AVRDUDE_WRITE_XBOOTLOADER)

# 3. write firmware/eeprom
$(RTBT) -p $(SERIAL_PORT)
sleep $(POST_RTBT_SLEEP_TIME)
$(AVRDUDE) $(AVRDUDE_OPT) -P $(SERIAL_PORT) -b $(BAUDRATE_XBOOTLOADER) -p $(AVRDUDE_MCU) -c $(AVRDUDE_PROGRAMMER_XBOOTLOADER) -U $(AVRDUDE_WRITE_FIRMWARE) -U $(AVRDUDE_WRITE_EEPROM)

# 4. run tests
# 4.1 bctool get version
$(BCTOOL) -p $(SERIAL_PORT) $(BCTOOL_OPT) -V
# 4.2 bctool get status
$(BCTOOL) -p $(SERIAL_PORT) $(BCTOOL_OPT) -s
# 4.3 bctool enable fpga sim, change logLevel
$(BCTOOL) -p $(SERIAL_PORT) $(BCTOOL_OPT) -T1 -O253
# 4.4 bctool view fpga sim data in GUI
$(BCTOOL) -p $(SERIAL_PORT) -g
# 4.5 bctool reset
$(BCTOOL) -p $(SERIAL_PORT) -r

# 5. flash fpga
$(FPGA_FLASH) -p $(SERIAL_PORT) $(FPGA_FLASH_OPT) -w -f $(FPGA_FLASH_IMG)

# 6. configure channel config (user)
$(BCTOOL) -p $(SERIAL_PORT) $(BCTOOL_OPT) -L $(CHANNEL_CONFIG_FILE)

