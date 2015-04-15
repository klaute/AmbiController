/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 * FSM states for binary communication protocol decoding
 *
 */

/*****************************************************************************/

#define BC_STATE_PROCESS_ANSWER_DATA      0x00
#define BC_STATE_PROCESS_RGB_LED_DATA     0x01
#define BC_STATE_PROCESS_STATUS           0x02
#define BC_STATE_PROCESS_YCBCR_FRAME_DATA 0x03
#define BC_STATE_READ_ANSWER              0x04
#define BC_STATE_READ_ANSWER_DATA         0x05
#define BC_STATE_READ_FIRST_CHAR          0x06
#define BC_STATE_READ_FIRST_END_CHAR      0x07
#define BC_STATE_READ_RGB_LED_DATA        0x08
#define BC_STATE_READ_RGB_LED_DATA1       0x09
#define BC_STATE_READ_RGB_LED_DATA2       0x0A
#define BC_STATE_READ_RGB_LED_DATA3       0x0B
#define BC_STATE_READ_RGB_LED_DATA4       0x0C
#define BC_STATE_READ_SECOND_CHAR         0x0D
#define BC_STATE_READ_SECOND_END_CHAR     0x0E
#define BC_STATE_READ_STATUS              0x0F
#define BC_STATE_READ_TYPE                0x10
#define BC_STATE_READ_YCBCR_FRAME_DATA    0x11
#define BC_STATE_READ_YCBCR_FRAME_DATA1   0x12
#define BC_STATE_READ_YCBCR_FRAME_DATA2   0x13
#define BC_STATE_READ_YCBCR_FRAME_DATA3   0x14
#define BC_STATE_READ_YCBCR_FRAME_DATA4   0x15

/*****************************************************************************/

