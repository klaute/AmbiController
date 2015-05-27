/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

/*****************************************************************************/

#include "gui_helper.h"

/*****************************************************************************/

unsigned char cmd_bytesToRead[BC_CMD_CNT];

/*****************************************************************************/

int msgState = BC_STATE_READ_FIRST_CHAR;
unsigned char nextStateToProcessData = BC_STATE_READ_FIRST_CHAR;
int cmd    = -1;
int type   = -1;
int status = -1;

/*****************************************************************************/

int comm_timeout = 0;
time_t time_old;

unsigned char waitForAmbiStatus = 0;
unsigned char waitForTVPStatus  = 0;
unsigned char waitForADVStatus  = 0;
unsigned char waitForAnswer     = 0;

/*****************************************************************************/

void pollCommFSM(void);

/*****************************************************************************/

void bc_init(void);

/*****************************************************************************/

void bc_resetAVR(void);
void bc_resetAVR_to_Bootloader(void);
void bc_nextTVPInput(void);
void bc_setNCRGB(int, int, int);
void bc_setMaxRGB(int, int, int);
void bc_setOffsetRGB(int, int, int);
void bc_setLPFC(int);
void bc_setDSPInSW(int);
void bc_setBrightness(int);
void bc_setAmbiStatus(int);
void bc_setLogLevel(int);
void bc_getAmbiStatus(void);
void bc_getADVStatus(void);
void bc_getTVPStatus(void);
void bc_setSearchSignal(int);
void bc_setSleepTimeout(int);
void bc_setFPGAYCbCrSimulation(int);
void bc_getDSPSel(void);
void bc_setChannelLEDNum(int, int, int, int, int);
void bc_sendChannelConfigData(int, unsigned int*, int);
void bc_enableTTYBinMode(int);
void bc_enableTFCMode(void);

/*****************************************************************************/

