/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */
 
 /* ------------------------------------------------------------------------- */
// EEPROM data positions

#define EEPROM_AMBICTRL_CFG_PAGE 0

// value to add to the color component value
#define EEPROM_R_OFFSET_ADDR 0
#define EEPROM_G_OFFSET_ADDR 1
#define EEPROM_B_OFFSET_ADDR 2

// shown color if no signal is connected
#define EEPROM_R_NOSIG_COLOR_ADDR 3
#define EEPROM_G_NOSIG_COLOR_ADDR 4
#define EEPROM_B_NOSIG_COLOR_ADDR 5

//  the maximum color value for the lights
#define EEPROM_R_MAX_ADDR 6
#define EEPROM_G_MAX_ADDR 7
#define EEPROM_B_MAX_ADDR 8

#define EEPROM_BRIGHTNESS_ADDR 9

#define EEPROM_EDGE_LED_CNT_A_ADDR 10
#define EEPROM_EDGE_LED_CNT_B_ADDR 11
#define EEPROM_EDGE_LED_CNT_C_ADDR 12
#define EEPROM_EDGE_LED_CNT_D_ADDR 13
#define EEPROM_EDGE_LED_CNT_E_ADDR 14

#define EEPROM_LOW_PASS_FILTER_C_ADDR_LOW  15
#define EEPROM_LOW_PASS_FILTER_C_ADDR_HIGH 16

#define EEPROM_SEARCH_FOR_INPUT_SIGNAL_ADDR 17

#define EEPROM_SLEEP_TIMEOUT_ADDR_LOW  18
#define EEPROM_SLEEP_TIMEOUT_ADDR_HIGH 19

// bootloader content - same defintion shoule be done in xbootloader ;)
#define EEPROM_OTHER_PAGE 1 //< Should be the same address as set in the XBootloader makefile

#define XBOOTLOADER_START_FLAG_ADDR 1 //< Should be the same address as set in the XBootloader makefile

// channel configuration
#define EEPROM_CHANNEL_A_DATA_PAGE  2 // 128 byte = space for 64 LEDs within 4 EEPROM pages (32 byte each)
#define EEPROM_CHANNEL_B_DATA_PAGE  6 // 192 byte = space for 96 LEDs within 6 EEPROM pages (32 byte each)
#define EEPROM_CHANNEL_C_DATA_PAGE 12 // 128 byte = space for 64 LEDs within 4 EEPROM pages (32 byte each)
#define EEPROM_CHANNEL_D_DATA_PAGE 16 //  96 byte = space for 48 LEDs within 3 EEPROM pages (32 byte each)
#define EEPROM_CHANNEL_E_DATA_PAGE 19 //  96 byte = space for 48 LEDs within 3 EEPROM pages (32 byte each)

#define EEPROM_CHANNEL_A_POSITIONS_START_ADDR 0 // start address in first page
#define EEPROM_CHANNEL_B_POSITIONS_START_ADDR 0 // start address in first page
#define EEPROM_CHANNEL_C_POSITIONS_START_ADDR 0 // start address in first page
#define EEPROM_CHANNEL_D_POSITIONS_START_ADDR 0 // start address in first page
#define EEPROM_CHANNEL_E_POSITIONS_START_ADDR 0 // start address in first page

//#define EEPROM_PAGE_SIZE 32 //< Page size as byte

/* ------------------------------------------------------------------------- */

