#include "doomgeneric.h"
#include "led-matrix-c.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

struct RGBLedMatrix *matrix;
struct LedCanvas *offscreen_canvas;
struct Color *scaled_pixels = NULL;

/*
dimensions for the drawing surface which may be
less than the matrix dimensions to account for aspect ratio
*/
int surfaceWidth, surfaceHeight;
int matrixWidth, matrixHeight;
int *x_scale_map = NULL;
int *y_scale_map = NULL;
uint32_t last_frame_ms = 0;

static void free_resources(void) {
    free(x_scale_map);
    free(y_scale_map);
    free(scaled_pixels);
    x_scale_map = NULL;
    y_scale_map = NULL;
    scaled_pixels = NULL;

    if (matrix != NULL) {
        led_matrix_delete(matrix);
        matrix = NULL;
    }
}

void catch_int(int sig_num) {
    free_resources();
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

    scaled_pixels = malloc((size_t)surfaceWidth * (size_t)surfaceHeight * sizeof(*scaled_pixels));
    x_scale_map = malloc((size_t)surfaceWidth * sizeof(*x_scale_map));
    y_scale_map = malloc((size_t)surfaceHeight * sizeof(*y_scale_map));

    if (scaled_pixels == NULL || x_scale_map == NULL || y_scale_map == NULL) {
        fprintf(stderr, "Failed to allocate render buffers\n");
        free_resources();
        exit(1);
    }

    for (int x = 0; x < surfaceWidth; ++x) {
        x_scale_map[x] = (x * DOOMGENERIC_RESX) / surfaceWidth;
    }

    for (int y = 0; y < surfaceHeight; ++y) {
        y_scale_map[y] = (y * DOOMGENERIC_RESY) / surfaceHeight;
    }
}

void DG_DrawFrame() {
    const uint32_t now_ms = DG_GetTicksMs();
    const uint32_t frame_interval_ms = 1000 / 35;

    if (last_frame_ms != 0 && now_ms - last_frame_ms < frame_interval_ms) {
        return;
    }

    last_frame_ms = now_ms;

    for (int y = 0; y < surfaceHeight; ++y) {
        const uint32_t *src_row = (const uint32_t *)DG_ScreenBuffer + y_scale_map[y] * DOOMGENERIC_RESX;
        struct Color *dst_row = scaled_pixels + y * surfaceWidth;

        for (int x = 0; x < surfaceWidth; ++x) {
            const uint32_t pixel = src_row[x_scale_map[x]];
            dst_row[x].r = pixel >> 16;
            dst_row[x].g = pixel >> 8;
            dst_row[x].b = pixel;
        }
    }

    if (surfaceWidth != matrixWidth || surfaceHeight != matrixHeight) {
        led_canvas_clear(offscreen_canvas);
    }

    led_canvas_set_pixels(offscreen_canvas, 0, 0, surfaceWidth, surfaceHeight, scaled_pixels);
    offscreen_canvas = led_matrix_swap_on_vsync(matrix, offscreen_canvas);
}

void DG_SleepMs(uint32_t ms) {
    usleep(ms * 1000);
}

uint32_t DG_GetTicksMs() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

int DG_GetKey(int* pressed, unsigned char* key) {
    return 0;
}

void DG_SetWindowTitle(const char* title) {
}
