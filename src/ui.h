#ifndef UI_H_
#define UI_H_

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>

typedef struct {
  SDL_Texture *texture;
  int width, height;
  float scale;
  float last_scale;
  int offset_x, offset_y;
  int dragging;
  int last_x, last_y;
} Image;

// Grab monitor size
int max_window_size(int *max_width, int *max_height);

// Initialize SDL
int init_sdl(SDL_Window **win, SDL_Renderer **renderer);

// Load image into buffer
int load_image_data(const char *filename, SDL_Renderer *renderer, Image *img, SDL_Window **win);

// rendering the zoom buttons
void render_buttons(SDL_Renderer *renderer);

// render the loaded image (with buttons)
void render_image(SDL_Renderer *renderer, Image *img, SDL_Window *win);

// handle zoom features
void handle_zoom(Image *img, int x, int y, float factor);

// handle mouse clicks
void handle_events(SDL_Event *event, Image *img, SDL_Window *win);

#endif
