#include "ui.h"

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("\nUsage: %s <image_path>\n\n", argv[0]);
    return -1;
  }

  SDL_Window *win = NULL;
  SDL_Renderer *renderer = NULL;
  if (init_sdl(&win, &renderer) < 0) {
    return -1;
  }

  Image img;
  if (load_image(argv[1], renderer, &img, &win) < 0) {
    return -1;
  }

  SDL_Event event;
  int running = 1;

  while (running) {
    while (SDL_PollEvent(&event)) {
      handle_events(&event, &img, win);
      if (event.type == SDL_QUIT) {
        running = 0;
      }
    }
    render_image(renderer, &img, win);
    SDL_Delay(16);
  }

  SDL_DestroyTexture(img.texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
  
  return 0;
}
