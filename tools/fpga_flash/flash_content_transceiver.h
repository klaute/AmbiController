/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 */

#define SPI_FLASH_CONTENT_MSG_SOD          0xAB
#define SPI_FLASH_CONTENT_MSG_EOD          0xBA
#define SPI_FLASH_CONTENT_MSG_ERR          0xEE

#define SPI_FLASH_CONTENT_ANSWER_OK        0x01
#define SPI_FLASH_CONTENT_ANSWER_NOK       0xFF

#define SPI_FLASH_CONTENT_CMD_WR_FLASH     0x01
#define SPI_FLASH_CONTENT_CMD_RD_FLASH     0x02
#define SPI_FLASH_CONTENT_CMD_WR_DONE      0x03
#define SPI_FLASH_CONTENT_CMD_RD_PAGE      0x04
#define SPI_FLASH_CONTENT_CMD_SECTOR_ERASE 0x05
#define SPI_FLASH_CONTENT_CMD_END          0xFF
