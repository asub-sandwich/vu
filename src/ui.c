#include "ui.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SDL_Rect zoom_in_button = { 20, 20, 30, 30 };
SDL_Rect zoom_out_button = { 20, 60, 30, 30 };


int init_sdl(SDL_Window **win, SDL_Renderer **renderer) {
  int max_width, max_height;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL Error: %s\n", SDL_GetError());
    return -1;
  }

  max_window_size(&max_width, &max_height);

  *win = SDL_CreateWindow(
    "Vu",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    max_width,
    max_height,
    SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
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

int load_image(const char *filename, SDL_Renderer *renderer, Image *img, SDL_Window **win) {
  int width, height, channels;
  int max_width, max_height;
  char *title;

  max_window_size(&max_width, &max_height);

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
    0x000000ff,
    0x0000ff00,
    0x00ff0000,
    0xff000000
  );
  if (!surface) {
    printf("SDL Error: %s", SDL_GetError());
    return -1;
  }

  img->texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (!img->texture) {
    printf("SDL Error: %s\n", SDL_GetError());
    SDL_FreeSurface(surface);
    stbi_image_free(data);
    return -1;
  }

  img->width = width;
  img->height = height;
  img->scale = 1.0f;
  img->last_scale = 1.0f;
  img->dragging = 0;

  SDL_FreeSurface(surface);
  stbi_image_free(data);

  float scale_x = (float)max_width / width;
  float scale_y = (float)max_height / height;
  float scale = (scale_x < 1.0f || scale_y < 1.0f) ? (scale_x < scale_y ? scale_x : scale_y) : 1.0f;

  img->scale = scale;
  img->offset_x = (max_width - width * scale);
  img->offset_y = (max_height - height * scale);

  int win_width = (width * scale > max_width) ? max_width : width * scale;
  int win_height = (height * scale > max_height) ? max_height : height * scale;

  if (asprintf(&title, "Vu: %s", filename) < 0) return -1;

  SDL_SetWindowSize(*win, win_width, win_height);
  SDL_SetWindowTitle(*win, title);
  free(title);

  return 0;
}

void render_buttons(SDL_Renderer *renderer) {
  int in_x = zoom_in_button.x;
  int in_y = zoom_in_button.y;
  int out_x = zoom_out_button.x;
  int out_y = zoom_out_button.y;


  // Draw Zoom In button
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_RenderFillRect(renderer, &zoom_in_button);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawLine(renderer, in_x + 10, in_y + 15, in_x + 20, in_y + 15);
  SDL_RenderDrawLine(renderer, in_x + 15, in_y + 10, in_x + 15, in_y + 20);

  // Draw Zoom Out Button
  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
  SDL_RenderFillRect(renderer, &zoom_out_button);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderDrawLine(renderer, out_x + 10, out_y + 15, out_x + 20, out_y + 15);  
}

void render_image(SDL_Renderer *renderer, Image *img, SDL_Window *win) {
  SDL_RenderClear(renderer);

  int w_width, w_height;
  SDL_GetWindowSize(win, &w_width, &w_height);

  int n_width = (int)(img->width * img->scale);
  int n_height = (int)(img->height * img->scale);

  if (n_width > w_width) n_width = w_width;
  if (n_height > w_height) n_height = w_height;

  SDL_Rect dst_rect = { img->offset_x, img->offset_y, n_width, n_height};
  SDL_RenderCopy(renderer, img->texture, NULL, &dst_rect);

  render_buttons(renderer);

  SDL_RenderPresent(renderer);
}

void handle_zoom(Image *img, int x, int y, float factor) {
  img->last_scale = img->scale;
  img->scale *= factor;

  if (img->scale < 0.1f) img->scale = 0.1f;

  float change = img->scale / img->last_scale;

  img->offset_x = x - (x - img->offset_x) * change;
  img->offset_y = y - (y - img->offset_y) * change;
}

void handle_events(SDL_Event *event, Image *img, SDL_Window *win) {
  int w_width, w_height;
  SDL_GetWindowSize(win, &w_width, &w_height);

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
        int x = event->button.x;
        int y = event->button.y;

        if (x >= zoom_in_button.x && x <= zoom_in_button.x + zoom_in_button.w &&
            y >= zoom_in_button.y && y <= zoom_in_button.y + zoom_in_button.h) 
          handle_zoom(img, x, y, 1.1f);

        if (x >= zoom_out_button.x && x <= zoom_out_button.x + zoom_out_button.w &&
            y >= zoom_out_button.y && y <= zoom_out_button.y + zoom_out_button.h)
          handle_zoom(img, x, y, 0.9f);

        img->dragging = 1;
        SDL_GetMouseState(&img->last_x, &img->last_y);
      }
      break;

    case SDL_MOUSEBUTTONUP:
      if (event->button.button == SDL_BUTTON_LEFT)
        img->dragging = 0;
      break;

    case SDL_MOUSEWHEEL:
      if (event->wheel.y != 0) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (event->wheel.y > 0) handle_zoom(img, x, y, 1.1f);
        else handle_zoom(img, x, y, 0.9f);
      }
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

int max_window_size(int *max_width, int *max_height) {
  
  SDL_DisplayMode mode;
  if (SDL_GetCurrentDisplayMode(0, &mode) != 0) {
    printf("Could not retrieve monitor size: %s\n", SDL_GetError());
    return -1;
  }
  max_width = &mode.w;
  max_height = &mode.h;
  
  return 0;
}
