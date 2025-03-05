#ifndef IO_H_
#define IO_H_

enum Driver {
  TIF,
  STB,
};

typedef struct {
  const char* filename;
  enum Driver driver;
  int width;
  int height;
  int channels;
  int depth;
  int is_tiled;
  int tile_width;
  int tile_height;
  unsigned char* data;
} ImageData;

ImageData* load_image(const char* filename);
void close_image(ImageData* image);
const char* fn_extension(const char* filename);
void to_lower(char* s);

#endif
