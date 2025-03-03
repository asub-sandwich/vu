#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

typedef struct {
  SDL_Texture *texture;
  int width, height;
  float scale;
  int offset_x, offset_y;
  int dragging;
  int last_x, last_y;
} Image;

int init_sdl(SDL_Window **win, SDL_Renderer **renderer) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  *win = SDL_CreateWindow(
    "Vu",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1200,
    800,
    SDL_WINDOW_RESIZABLE|SDL_WINDOW_SHOWN
  );
  if (!*win) {
    printf("SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  *renderer = SDL_CreateRenderer(*win, -1, SDL_RENDERER_ACCELERATED);
  if (!*renderer) {
    printf("SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  return 0;
}

int load_image(const char *filename, SDL_Renderer *renderer, Image *img) {
  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
  if (!data) {
    printf("Failed to load image: %s\n", filename);
    return -1;
  }

  SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(
    data,
    width,
    height,
    channels * 8,
    width * channels,
    0x000000FF,
    0x0000FF00,
    0x00FF0000,
    0xFF000000
  );
  if (!surface) {
    printf("Failed to create surface from image: %s\n", SDL_GetError());
    stbi_image_free(data);
    return -1;
  }

  img->texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!img->texture) {
    printf("Failed to create texture: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    stbi_image_free(data);
    return -1;
  }

  img->width = width;
  img->height = height;
  img->scale = 1.0f;
  img->offset_x = 0;
  img->offset_y = 0;
  img->dragging = 0;
  SDL_FreeSurface(surface);
  stbi_image_free(data);
  
  return 0;
}

void render_image(SDL_Renderer *renderer, Image *img, SDL_Window *win) {
  SDL_RenderClear(renderer);

  int window_width, window_height;
  SDL_GetWindowSize(win, &window_width, &window_height);

  int new_width = (int)(img->width * img->scale);
  int new_height = (int)(img->height * img->scale);

  if (new_width > window_width) new_width = window_width;
  if (new_height > window_height) new_height = window_height;

  SDL_Rect dst_rect = { img->offset_x, img->offset_y, new_width, new_height };
  SDL_RenderCopy(renderer, img->texture, NULL, &dst_rect);

  SDL_RenderPresent(renderer);
}

void handle_events(SDL_Event *event, Image *img, SDL_Window *win) {
  int window_width, window_height;
  SDL_GetWindowSize(win, &window_width, &window_height);

  switch (event->type) {
    case SDL_QUIT:
      break;

    case SDL_MOUSEMOTION:
      if (img->dragging) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        img->offset_x += x - img->last_x;
        img->offset_y += y - img->last_y;
        img->last_x = x;
        img->last_y = y;
      }
      break;

    case SDL_MOUSEBUTTONDOWN:
      if (event->button.button == SDL_BUTTON_LEFT) {
        img->dragging = 1;
        SDL_GetMouseState(&img->last_x, &img->last_y);
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event->button.button == SDL_BUTTON_LEFT) {
        img->dragging = 0;
      }
      break;

    case SDL_MOUSEWHEEL:
      if (event->wheel.y > 0) {
        img->scale *= 1.1f;
      } else {
        img->scale /= 1.1f;
      }

      if (img->scale < 0.1f) img->scale = 0.1f;
      if (img->scale > 10.0f) img->scale = 10.0f;
      break;

    case SDL_WINDOWEVENT:
      if (event->window.event == SDL_WINDOWEVENT_RESIZED) {
        int width, height;
        SDL_GetWindowSize(win, &width, &height);
        if (img->offset_x + img->width * img->scale > width) img->offset_x = width - img->width * img->scale;
        if (img->offset_y + img->height * img->scale > height) img->offset_y = height - img->height * img->scale;
      }
      break;

    default:
      break;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <image_path>", argv[0]);
    return -1;
  }

  SDL_Window *win = NULL;
  SDL_Renderer *renderer = NULL;
  if (init_sdl(&win, &renderer) < 0) {
    return -1;
  }

  Image img;
  if (load_image(argv[1], renderer, &img) < 0) {
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
