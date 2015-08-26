/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

/*****************************************************************************/

//#define DEBUG
//#define DEBUG2
#define ENABLE_SDL
#define USE_THREADS

/*****************************************************************************/

#define SERIAL_HELPER_BAUDRATE     B460800
#define SERIAL_HELPER_BAUDRATE_USB B921600

/*****************************************************************************/

#define WINDOW_WIDTH  600 //< Pixel
#define WINDOW_HEIGHT (uint16_t)((WINDOW_WIDTH*3)/4) //< create a window with 4:3 propotion

#define BORDER_WIDTH  20 //< Pixel
#define BORDER_HEIGHT 20 //< Pixel

#define LED_COLOR_PIXE_WIDTH  10
#define LED_COLOR_PIXE_HEIGHT 10

#define SPACE_BETWEEN_PIXELDATA_HALF 1

/*****************************************************************************/

#define DELAY_READ_POLL_FSM_US (unsigned int)(10) //< No delay active (higest performance).

/*****************************************************************************/

#define COMMUNICATION_TIMEOUT_SEC 5 // 5 seconds until the read process stops to wait for incomeing data (in command line mode)

/*****************************************************************************/
// the count of bytes which should be read related to the input command

#define BC_CMD_GET_AMBI_STATUS_RECV_BYTE     19
#define BC_CMD_TVP_GET_STATUS_RECV_BYTE       4
#define BC_CMD_ADV_GET_STATUS_RECV_BYTE       3
#define BC_CMD_SET_AMBI_STATUS_RECV_BYTE      0
#define BC_CMD_SET_LOGLEVEL_RECV_BYTE         0
#define BC_CMD_SET_SIG_SEARCH_RECV_BYTE       0
#define BC_CMD_SET_SLEEP_TOUT_RECV_BYTE       0
#define BC_CMD_SET_NC_RGB_RECV_BYTE           0
#define BC_CMD_SET_OFFSET_RGB_RECV_BYTE       0
#define BC_CMD_SET_MAX_RGB_RECV_BYTE          0
#define BC_CMD_SET_BRIGHTNESS_RECV_BYTE       0
#define BC_CMD_SET_LPF_RECV_BYTE              0
#define BC_CMD_TVP_NEXT_INPUT_RECV_BYTE       1 // signal source ID
#define BC_CMD_DSP_SEL_RECV_BYTE              0
#define BC_CMD_RESET_AVR_RECV_BYTE            0
#define BC_CMD_RESET_TO_BTLDR_AVR_RECV_BYTE   0
#define BC_CMD_GET_VERSION_INFO_RECV_BYTE    24
#define BC_CMD_SET_SIM_YCBCR_DATA_RECV_BYTE   0
#define BC_CMD_GET_DSP_SEL_RECV_BYTE          1
#define BC_CMD_SET_CHANNEL_LED_NUM_RECV_BYTE  0
#define BC_CMD_SET_CHANNEL_DATA_RECV_BYTE     0

/*****************************************************************************/

#ifdef SDL_DISABLED_BY_MAKEFILE
#undef ENABLE_SDL
#endif

/*****************************************************************************/

