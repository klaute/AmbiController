/*
 * (C) 2014 by Kai Lauterbach (lauterbach.kai@web.de)
 *
 * License: General Public License (GPL v3)
 *
 */

#ifdef ENABLE_SDL

/*****************************************************************************/

#include "gui_helper.h"

/*****************************************************************************/

void gui_init_window()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Error: SDL konnte nicht initialisiert werden: %s\n", SDL_GetError());

        exit_all();

        exit(1);
    }

    atexit(exit_all); // set exit action (call the exit_all function)

    sdlWindow = SDL_CreateWindow("FrameView/Binary communication tool",
                                 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // position
                                 WINDOW_WIDTH, WINDOW_HEIGHT, // size
                                 SDL_WINDOW_OPENGL); // simple OpenGL window
                                 //SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, 0);

    sdlSurface = SDL_GetWindowSurface(sdlWindow);

    if (!sdlWindow) {
        fprintf(stderr, "Error: Konnte Bildschirmmodus nicht setzen: %s\n", SDL_GetError());
            
        exit_all();

        exit(1);
    }

    //gui_pixel_test();

}

/*****************************************************************************/

void gui_clear_display()
{
    uint8_t x, y = 0;
    for (x = 0; x < FRAME_WIDTH; x++)
    {
        for (y = 0; y < FRAME_WIDTH; y++)
        {
            gui_show_YCbCr_pixel(x, y, 0, 0, 0);
        }
    }
}

void gui_pixel_test()
{
    uint8_t x, y, r, g, b = 0;
    for (x = 0; x < FRAME_WIDTH; x++)
    {
        for (y = 0; y < FRAME_WIDTH; y++)
        {
            if (x < 6)
            {
                r = 255;
                g = 0;
                b = 0;
            } else if (x < 12) {
                r = 0;
                g = 255;
                b = 0;
            } else {
                r = 0;
                g = 0;
                b = 255;
            }
#ifdef DEBUG
            printf("x = %d y = %d r = %d g = %d b = %d\n", x, y, r, g, b);
#endif
            gui_show_YCbCr_pixel(x, y, r, g, b);
            //usleep(200000); // sleep 200 ms
        }
    }
}

/*****************************************************************************/

void gui_show_RGB_pixel(unsigned int led, unsigned int r, unsigned int g, unsigned int b)
{

    SDL_Rect rec;
    int px, py = 0;

    // draw new color
    rec.x = 0;
    rec.y = 0;
    rec.w = LED_COLOR_PIXE_WIDTH;
    rec.h = LED_COLOR_PIXE_HEIGHT;

    int gChannelALEDNum = 18;
    int gChannelBLEDNum = 34;
    int gChannelCLEDNum = 18;
    int gChannelDLEDNum = 15;
    int gChannelELEDNum = 15;

    //if (led < 24)
    if (led < gChannelALEDNum)
    {
        // left bottom to top
        px = 2;
        py =   (unsigned int)(gChannelALEDNum - led) * (((float)FRAME_WIDTH+0.0) * (float)(WINDOW_HEIGHT / ((float)FRAME_WIDTH + 3.7)) / (float)(gChannelALEDNum))
             + (1 * WINDOW_HEIGHT / (FRAME_WIDTH + 4)) + 5;
    } else if (led < gChannelALEDNum + gChannelBLEDNum)
    {
        // top left to right
        px =   (unsigned int)(led - gChannelALEDNum) * (((float)FRAME_WIDTH+0.0) * (float)(WINDOW_WIDTH / ((float)FRAME_WIDTH + 3.7)) / (float)(gChannelBLEDNum))
             + (2 * WINDOW_WIDTH / (FRAME_WIDTH + 4)) + 2;
        py = 2;
    } else if (led < gChannelALEDNum + gChannelBLEDNum + gChannelCLEDNum)
    {
        // right top to down
        px = WINDOW_WIDTH - (WINDOW_WIDTH / gChannelBLEDNum) - 2;
        py =   (unsigned int)(led - (gChannelALEDNum + gChannelBLEDNum)) * (((float)FRAME_WIDTH+0.0) * (float)(WINDOW_HEIGHT / ((float)FRAME_WIDTH + 3.7)) / (float)(gChannelCLEDNum))
             + (2 * WINDOW_HEIGHT / (FRAME_WIDTH + 4)) + 1;
    } else {
        // bottom right to left
        px =   (unsigned int)((gChannelDLEDNum + gChannelELEDNum - 1) - (led - (gChannelALEDNum + gChannelBLEDNum + gChannelCLEDNum))) * (((float)FRAME_WIDTH+0.0) * (float)(WINDOW_WIDTH / ((float)FRAME_WIDTH + 3.7)) / (float)(gChannelDLEDNum + gChannelELEDNum))
             + (2 * WINDOW_WIDTH / (FRAME_WIDTH + 4)) + 2;
        py = WINDOW_HEIGHT - (WINDOW_HEIGHT / gChannelCLEDNum) - 2;
    }

    rec.x = px;
    rec.y = py;


#ifdef DEBUG
    printf("LED-Pixel: x = %d y = %d w = %d h = %d\n", rec.x, rec.y, rec.w, rec.h);
#endif

    SDL_FillRect(sdlSurface, &rec, SDL_MapRGB(sdlSurface->format, r, g, b) );

    // update sdlWindow
    SDL_UpdateWindowSurface(sdlWindow);
}

/*****************************************************************************/

void gui_show_YCbCr_pixel(unsigned int px, unsigned int py, unsigned int r, unsigned int g, unsigned int b)
{
    SDL_Rect rec;

    // draw new color
    rec.x = BORDER_WIDTH  + SPACE_BETWEEN_PIXELDATA_HALF + (px * (WINDOW_WIDTH  - 2 * BORDER_WIDTH ) / FRAME_WIDTH);
    rec.y = BORDER_HEIGHT + SPACE_BETWEEN_PIXELDATA_HALF + (py * (WINDOW_HEIGHT - 2 * BORDER_HEIGHT) / FRAME_WIDTH);
    rec.w = ((WINDOW_WIDTH  - 2 * BORDER_WIDTH)  / FRAME_WIDTH) - SPACE_BETWEEN_PIXELDATA_HALF * 2;
    rec.h = ((WINDOW_HEIGHT - 2 * BORDER_HEIGHT) / FRAME_WIDTH) - SPACE_BETWEEN_PIXELDATA_HALF * 2;

#ifdef DEBUG
    printf("YCbCr-Pixel: x = %d y = %d w = %d h = %d\n", rec.x, rec.y, rec.w, rec.h);
#endif

    SDL_FillRect(sdlSurface, &rec, SDL_MapRGB(sdlSurface->format, r, g, b) );

    // update sdlWindow
    SDL_UpdateWindowSurface(sdlWindow);
}

/*****************************************************************************/

void gui_pollSDLWindowFunctionality()
{ 

    if (! gui_enabled)
        return;

    SDL_Event event;

    SDL_PollEvent(&event);

    switch(event.type)
    {
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_n) // Skip to the next TVP video input channel.
            {
                printf("Skip to next video input\n");
                bc_nextTVPInput();
            } else if (event.key.keysym.sym == SDLK_r) // Reset the AVR
            {
                printf("Reset the AVR\n");
                bc_resetAVR();
            } else if (event.key.keysym.sym == SDLK_b) // Reset the AVR for firmware update
            {
                printf("Reset the AVR for firmware update\n");
                bc_resetAVR_to_Bootloader();
            } else if (event.key.keysym.sym == SDLK_s) // Set switch to TVP as video input
            {
                printf("DSP_IN_SW 0\n");
                bc_setDSPInSW(0);
            } else if (event.key.keysym.sym == SDLK_d) // switch to ADV as video input
            {
                printf("DSP_IN_SW 1\n");
                bc_setDSPInSW(1);
            } else if (event.key.keysym.sym == SDLK_w) // enable ambilight processing
            {
                printf("SET_AMBI_STATUS 1\n");
                bc_setAmbiStatus(1);
            } else if (event.key.keysym.sym == SDLK_e) // disable ambilight processing
            {
                printf("SET_AMBI_STATUS 0\n");
                bc_setAmbiStatus(0);
            } else if (event.key.keysym.sym == SDLK_a) // read the ADV status
            {
                printf("GET_ADV_STATUS\n");
                bc_getADVStatus();
            } else if (event.key.keysym.sym == SDLK_t) // read the TVP status
            {
                printf("GET_TVP_STATUS\n");
                bc_getTVPStatus();
            } else if (event.key.keysym.sym == SDLK_f) // read the ambilight status
            {
                printf("GET_AMBI_STATUS\n");
                bc_getAmbiStatus();
            } else if (event.key.keysym.sym == SDLK_i) // set the loglevel to 253 (send RGB and YCbCr pixel data)
            {
                printf("SET_LOGLEVEL 253\n");
                bc_setLogLevel(253);
            } else if (event.key.keysym.sym == SDLK_l) // set the loglevel to 252 (send RGB pixel data)
            {
                printf("SET_LOGLEVEL 252\n");
                bc_setLogLevel(252);
            } else if (event.key.keysym.sym == SDLK_o) // set the loglevel to 255 (send YCbCr pixel data)
            {
                printf("SET_LOGLEVEL 255\n");
                bc_setLogLevel(255);
            } else if (event.key.keysym.sym == SDLK_c) // clear the display
            {
                printf("Clear window\n");
                gui_clear_display();
            } else if (event.key.keysym.sym == SDLK_p) // set the loglevel to 254 (normal mode, no pixel data will be send)
            {
                printf("SET_LOGLEVEL 254\n");
                bc_setLogLevel(254);
            } else if (event.key.keysym.sym == SDLK_m) // set the FPGA data simulation flag
            {
                printf("SET_FPGA_SIM 1\n");
                bc_setFPGAYCbCrSimulation(1);
            } else if (event.key.keysym.sym == SDLK_q) // Exit the FrameView executable.
                running = 0; // stop executable

            break;
        case SDL_QUIT:
            running = 0;
            break;
    }

}

/*****************************************************************************/

#endif

