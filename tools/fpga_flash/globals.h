/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 */

/******************************************************************************/

//#define DEBUG there are no debug messsages implemented yet

/******************************************************************************/

#define SERIAL_HELPER_BAUDRATE B921600
#define SERIAL_HELPER_BAUDRATE_USB SERIAL_HELPER_BAUDRATE

/******************************************************************************/

#define READ_DATA_TIMEOUT_COUNTS 200

/******************************************************************************/

#define SPI_FLASH_CONTENT_STATE_WAIT_FOR_SOD    0x00
#define SPI_FLASH_CONTENT_STATE_READ_USART_DATA 0x01
#define SPI_FLASH_CONTENT_STATE_WAIT_FOR_EOD    0x02

/******************************************************************************/

#define FLASH_LAST_ADDRESS      0x1FFFFF
#define FLASH_PAGE_SIZE              256
#define FLASH_PAGE_CNT              8192
#define ADDRESS_SIZE_BYTE              3

/******************************************************************************/

#define WAIT_FOR_OK_FOUND_FLAG          1
#define WAIT_FOR_OK_NOT_FOUND_FLAG      0
#define WAIT_FOR_OK_UNKNOWN_FLAG   0xFF00
#define WAIT_FOR_OK_TIMEOUT_FLAG   0xF000

/******************************************************************************/

#define WAIT_FOR_OK_TIMEOUT_COUNTS   2000

/******************************************************************************/

#ifdef USE_THREADS
#undef USE_THREADS
#warning THREADS disabled
#endif

/******************************************************************************/

#define uint8_t  unsigned char
#define uint16_t unsigned int
#define uint32_t unsigned int

/******************************************************************************/

