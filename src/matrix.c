#include "doomgeneric.h"
#include "led-matrix-c.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <SDL.h>

SDL_Surface* surface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

struct RGBLedMatrix *matrix;
struct LedCanvas *offscreen_canvas;

/*
dimensions for the drawing surface which may be
less than the matrix dimensions to account for aspect ratio
*/
int surfaceWidth, surfaceHeight;
int matrixWidth, matrixHeight;

void catch_int(int sig_num) {
    led_matrix_delete(matrix);
    exit(0);
}

int main(int argc, char **argv) {
  struct RGBLedMatrixOptions options;

  memset(&options, 0, sizeof(options));

  matrix = led_matrix_create_from_options(&options, &argc, &argv);
  if (matrix == NULL)
    return 1;

  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);
  led_canvas_get_size(offscreen_canvas, &matrixWidth, &matrixHeight);
  fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
          matrixWidth, matrixHeight, options.hardware_mapping);

  doomgeneric_Create(argc, argv);
  signal(SIGINT, catch_int);
  while(true) {
      doomgeneric_Tick();
  }

  return 0;
}

void DG_Init() {
    surfaceWidth = matrixWidth;
    surfaceHeight = matrixWidth * 0.625;

    if (surfaceHeight > matrixHeight) {
        surfaceHeight = matrixHeight;
        surfaceWidth = surfaceHeight / 0.625;
    }

    surface = SDL_CreateRGBSurface(0, surfaceWidth, surfaceHeight, 32, 0, 0, 0, 0);
    renderer = SDL_CreateSoftwareRenderer(surface);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

void DG_DrawFrame() {
    SDL_UpdateTexture(texture, NULL, DG_ScreenBuffer, DOOMGENERIC_RESX*sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    uint32_t *pix = surface->pixels;
    for (int y = 0; y < surfaceHeight; ++y) {
        for (int x = 0; x < surfaceWidth; ++x) {
            uint8_t r = *pix >> 16;
            uint8_t g = *pix >> 8;
            uint8_t b = *pix;
            led_canvas_set_pixel(offscreen_canvas, x, y, r, g, b);
            pix++;
        }
    }
    offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
}

void DG_SleepMs(uint32_t ms) {
    SDL_Delay(ms);
}

uint32_t DG_GetTicksMs() {
    return SDL_GetTicks(); 
}

int DG_GetKey(int* pressed, unsigned char* key) {
    return 0;
}

void DG_SetWindowTitle(const char* title) {
}

