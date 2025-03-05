#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiff.h>
#include <tiffio.h>
#include <xtiffio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

char* driver_name(enum Driver driver) {
  switch (driver) {
    case STB: return "STB";
    case TIF: return "TIF";
    default: return "";
  }
}

void pimg_info(int w, int h, int c, int d, enum Driver driver) {
  printf("Driver:    %s\n", driver_name(driver));
  printf("Width:     %d\n", w);
  printf("Height:    %d\n", h);
  printf("Channels:  %d\n", c);
  printf("Bit Depth: %d\n", d);
}


unsigned char* load_tiff_32t(TIFF* tif, int w, int h, int c, int tw, int th) {
  unsigned char* rgba_buf = (unsigned char*)malloc(w * h * 4);
  if (!rgba_buf) {
    printf("Could not malloc RGBA buffer\n");
    return NULL;
  }

  int pm;
  TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &pm);

  // RGBA
  // TODO: There is probably a better way to do this
  if (pm == PHOTOMETRIC_RGB && c == 4) {
    unsigned int* tile_buf = (unsigned int*)malloc(tw*th*sizeof(unsigned int));
    if (!tile_buf) {
      printf("Could not allocate tile buffer\n");
      free(rgba_buf);
      return NULL;
    }
    for (int ty=0; ty<h; ty+=th) {
      for (int tx=0; tx<w; tx+=tw) {
        if (TIFFReadRGBATile(tif, tx, ty, tile_buf)) {
          for (int j=0; j<th; j++) {
            for (int i=0; i<tw; i++) {
              int img_x = tx + i;
              int img_y = ty + j;
              if (img_x >= w || img_y >= h) continue;
              int tile_index = j * tw + i;
              int img_index = (img_y * w + img_x) * 4;

              unsigned int pixel = tile_buf[tile_index];
              rgba_buf[img_index + 0] = TIFFGetR(pixel);
              rgba_buf[img_index + 1] = TIFFGetG(pixel);
              rgba_buf[img_index + 2] = TIFFGetB(pixel);
              rgba_buf[img_index + 3] = TIFFGetA(pixel);
            }
          }
        }
        free(tile_buf);
      }
    }
    return rgba_buf;
  }

  float min, max;
  TIFFGetField(tif, TIFFTAG_MINSAMPLEVALUE, &min);
  TIFFGetField(tif, TIFFTAG_MAXSAMPLEVALUE, &max);
  
  int* tile_buf = (int*)malloc(tw * th * c * sizeof(int));
  if (!tile_buf) {
    printf("Could not malloc tile buffer\n");
    free(rgba_buf);
    return NULL;
  }

  for (int ty=0; ty<h; ty+=th) {
    for (int tx=0; tx<w; tx+=tw) {
      if (TIFFReadTile(tif, tile_buf, tx, ty, 0, 0) == -1) continue;

      for (int j=0; j<th; j++) {
        for (int i=0; i<tw; i++) {
          int img_x = tx + i;
          int img_y = ty + j;
          if (img_x >= w || img_y >= h) continue;

          int tile_index = (j * tw + i) * c;
          int img_index = (img_y * w + img_x) * 4;

          // RGB -> RGBA
          if (c == 3) {
            rgba_buf[img_index + 0] = (unsigned char)tile_buf[tile_index + 0];
            rgba_buf[img_index + 1] = (unsigned char)tile_buf[tile_index + 1];
            rgba_buf[img_index + 2] = (unsigned char)tile_buf[tile_index + 2];
            rgba_buf[img_index + 3] = 255; // Solid alpha channel
          } else if (c == 1) {
            float norm_val = (tile_buf[tile_index] - min) / (max - min);
            unsigned char grey = (unsigned char)(norm_val * 255.0f);
            rgba_buf[img_index + 0] = grey;
            rgba_buf[img_index + 1] = grey;
            rgba_buf[img_index + 2] = grey;
            rgba_buf[img_index + 3] = 255;
          }
        }
      }
    }
  }
  free(tile_buf);
  return rgba_buf;
}

void load_tiff_32() {}

void load_tiff_std() {}

void load_tiff_stdt() {}

ImageData* load_tiff(const char* filename) {
  TIFF* tif = XTIFFOpen(filename, "r");
  if (!tif) {
    printf("Could not open tif: %s\n", filename);
    return NULL;
  }

  ImageData* image;
  int w, h, c, d, t, tw = 0, th = 0;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &c);
  TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &d);
  t = TIFFIsTiled(tif);
  if (t) {
    TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
    TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
  }

  pimg_info(w, h, c, d, TIF);
  
  image = (ImageData*)malloc(sizeof(ImageData));

  image->filename = filename;
  image->driver = TIF;
  image->width = w;
  image->height = h;
  image->channels = c;
  image->depth = d;
  image->is_tiled = t;
  image->tile_width = tw;
  image->tile_height = th;

  if (d == 32 && t) {
    unsigned char* data = load_tiff_32t(tif, w, h, c, tw, th);
    if (!data) {
      printf("Failure loading 32 bit (tiled) tif\n");
      free(image);
      return NULL;
    }
    image->data = data;
  }
  else if (d == 32 && !t) {
    printf("32 bit (untiled) tiff not yet supported\n");
    free(image);
    XTIFFClose(tif);
    return NULL;
  }
  else if (t) {
    printf("%d bit (tiled) tiff not yet supported\n", d);
    free(image);
    XTIFFClose(tif);
    return NULL;
  }
  else {
    printf("%d bit (tiled) tiff not yet supportedn\n", d);
    free(image);
    XTIFFClose(tif);
    return NULL;
  }

  XTIFFClose(tif);
  return image;
}

ImageData* load_std(const char* filename) {
  ImageData* image;
  int w, h, c;
  image = (ImageData*)malloc(sizeof(ImageData));
  if (!image) {
    printf("Could not malloc for STB\n");
    return NULL;
  }

  
  image->filename = filename;
  image->driver = STB;
  image->data = stbi_load(filename, &w, &h, &c, 4);
  image->width = w;
  image->height = h;
  image->channels = c;
  image->depth = 0;
  image->is_tiled = 0;
  image->tile_width = 0;
  image->tile_height = 0;

  pimg_info(w, h, c, 32, STB);

  return image;
} 

ImageData* load_image(const char* filename) {

  ImageData* image;

  const char* ext = fn_extension(filename);
  
  if (!strcmp(ext, "tiff") || !strcmp(ext, "tif") || !strcmp(ext, "TIFF") || !strcmp(ext, "TIF")) {
    image = load_tiff(filename);
  } else {
    image = load_std(filename);
  }

  return image;
  
}

void close_image(ImageData* image) {
  if (image->data) {
    free(image->data);
    image->data = NULL;
  }
  if (image) {
    free(image);
    image = NULL;
  }
}

const char* fn_extension(const char* filename) {
  if (!filename || !*filename) return "";
  const char* dot = strrchr(filename, '.');
  if (!dot || dot == filename) return "";
  return (char*)(dot + 1);
}
