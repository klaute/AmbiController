/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

/*****************************************************************************/

#include "globals.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#ifdef ENABLE_SDL
#include <SDL.h>
#endif

#include <argtable2.h>

#ifdef USE_THREADS
#include <pthread.h>
#endif

#include "cfg_hw_synth.h"
#include "cfg_matrix.h"
#include "cfg_eeprom_content.h"

#include "bc_states.h"
#include "bc_message_defs.h"
#include "bc_message_types.h"
#include "bc_commands.h"
#include "bc_config.h"

/*****************************************************************************/

//#if WIN32
#define uint8_t  unsigned char
#define uint16_t unsigned  int
#define uint32_t unsigned  int
//#endif

/*****************************************************************************/

#undef main

/*****************************************************************************/

int gui_enabled = 0;

#ifdef ENABLE_SDL
SDL_Surface   *sdlSurface;
SDL_Window     *sdlWindow;
SDL_Renderer *sdlRenderer;
#endif

/*****************************************************************************/

#ifdef USE_THREADS
pthread_t *tdataReceiver;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int retDataReader;
int dataReaderThreadExit = 0;
#endif

/*****************************************************************************/

unsigned int running = 1;

/*****************************************************************************/

void* dataReaderThreadFunction(void*);

void exit_all(void);

int convertRGB(int, int, int);

void showGUIHelp(void);

void readAndSendLEDChannelCfg(char*);


/*****************************************************************************/

