/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */
 
 /* ------------------------------------------------------------------------- */
// Color channel definitions

/*
               B
  |------------------------|
  |                        |
  |                        |
A |                        | C
  |                        |
  |                        |
  |                        |
  |------------------------|
          E        D

*/

// 64 + 96 + 64 + 48 + 48 = 320 LEDs => 320 LEDs * 1,6666 cm = 533,3 cm
// 2meter = 120 LEDs = 32 inch TV => 5,26meter = ~54 inch TV
#define CHANNEL_A_LED_MAXIMUM 64
#define CHANNEL_B_LED_MAXIMUM 96
#define CHANNEL_C_LED_MAXIMUM 64
#define CHANNEL_D_LED_MAXIMUM 48
#define CHANNEL_E_LED_MAXIMUM 48

#ifdef ENABLE_OLD_COLOR_PROCESSING
// old definition for updataColors_old function
#define EDGE_PIXEL_CNT_A  MATRIX_EDGE_LENGTH       /**< */
#define EDGE_PIXEL_CNT_B  MATRIX_EDGE_LENGTH       /**< */
#define EDGE_PIXEL_CNT_C  MATRIX_EDGE_LENGTH       /**< */
#define EDGE_PIXEL_CNT_D (MATRIX_EDGE_LENGTH >> 1) /**< */
#define EDGE_PIXEL_CNT_E (MATRIX_EDGE_LENGTH >> 1) /**< */
#define EDGE_PIXEL_SUM    EDGE_PIXEL_CNT_A + EDGE_PIXEL_CNT_B + EDGE_PIXEL_CNT_C + EDGE_PIXEL_CNT_D + EDGE_PIXEL_CNT_E
#endif

/* ------------------------------------------------------------------------- */

#define NUM_LED     (uint16_t)(channel_cfg_A + channel_cfg_B + channel_cfg_C + channel_cfg_D + channel_cfg_E)
#define NUM_LED_MAX (uint16_t)(CHANNEL_A_LED_MAXIMUM + CHANNEL_B_LED_MAXIMUM + CHANNEL_C_LED_MAXIMUM + CHANNEL_D_LED_MAXIMUM + CHANNEL_E_LED_MAXIMUM)

/* ------------------------------------------------------------------------- */

#define FRAME_BUFFER       2 /*< Don't touch it!!! Don't even think about it... */

/* ------------------------------------------------------------------------- */

#define DETAULT_LOW_PASS_C 0xffd0

/* ------------------------------------------------------------------------- */

#ifdef ENABLE_CHANNEL_LOW_PASS_FILTER

#define FRAME_OUT_POS    FRAME_BUFFER+1 /*< Position in buffer array to send to the lights. */
#define FRAME_LP_OLD_POS FRAME_OUT_POS  /*< Position for lowpass old value. */

#else

#define FRAME_OUT_POS    FRAME_BUFFER   /*< Position in buffer array to send to the lights. */

#endif

/* ------------------------------------------------------------------------- */

#define MATRIX_EDGE_LENGTH     FRAME_WIDTH /**< The destination frame size of one edge, defined in config.h */
#define MATRIX_EDGE_LENGTH_X_2 MATRIX_EDGE_LENGTH * 2
#define MATRIX_EDGE_LENGTH_X_3 MATRIX_EDGE_LENGTH * 3
#define MATRIX_EDGE_LENGTH_X_4 MATRIX_EDGE_LENGTH * 4
#define MATRIX_EDGE_LENGTH_X_5 MATRIX_EDGE_LENGTH * 5
#define MATRIX_EDGE_LENGTH_X_6 MATRIX_EDGE_LENGTH * 6

#define MATRIX_DEST_SIZE     (uint16_t)(MATRIX_EDGE_LENGTH * MATRIX_EDGE_LENGTH)
#define MATRIX_DEST_SIZE_X_2 (uint16_t)(MATRIX_DEST_SIZE * 2)
#define MATRIX_DEST_SIZE_X_3 (uint16_t)(MATRIX_DEST_SIZE * 3)

#define FIELD_SIZE_PAL  720 * 312 // 720 * (625/2)
#define FIELD_SIZE_NTSC 720 * 262 // 720 * (525/2)

#define FIELD_BLOCK_HEIGHT (720 / MATRIX_EDGE_LENGTH)
#define FIELD_BLOCK_WIDTH  (312 / MATRIX_EDGE_LENGTH)

/* ------------------------------------------------------------------------- */

