#include "doomgeneric.h"
#include "led-matrix-c.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <SDL.h>

SDL_Surface* surface = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture;
struct RGBLedMatrix *matrix;
struct LedCanvas *offscreen_canvas;
int width, height, aspectHeight;

void catch_int(int sig_num) {
    led_matrix_delete(matrix);
}

int main(int argc, char **argv) {
  struct RGBLedMatrixOptions options;
  int x, y, i;

  memset(&options, 0, sizeof(options));
  options.chain_length = 1;

  matrix = led_matrix_create_from_options(&options, &argc, &argv);
  if (matrix == NULL)
    return 1;

  offscreen_canvas = led_matrix_create_offscreen_canvas(matrix);
  led_canvas_get_size(offscreen_canvas, &width, &height);
  fprintf(stderr, "Size: %dx%d. Hardware gpio mapping: %s\n",
          width, height, options.hardware_mapping);

  doomgeneric_Create(argc, argv);
  signal(SIGINT, catch_int);
  for (int i = 0; ; i++)
  {
      doomgeneric_Tick();
  }

  return 0;
}

void DG_Init() {
    aspectHeight = width * 0.625;
    surface = SDL_CreateRGBSurface(0, width, aspectHeight, 32, 0, 0, 0, 0);
    renderer = SDL_CreateSoftwareRenderer(surface);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, DOOMGENERIC_RESX, DOOMGENERIC_RESY);
}

void DG_DrawFrame() {
    SDL_UpdateTexture(texture, NULL, DG_ScreenBuffer, DOOMGENERIC_RESX*sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    uint32_t *pix = surface->pixels;
    for (int y = 0; y < aspectHeight; ++y) {
        for (int x = 0; x < width; ++x) {
            uint32_t pixel = *pix;
            uint8_t r = (pixel >> 16) & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t b = pixel & 0xFF;
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

