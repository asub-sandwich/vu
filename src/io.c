#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tiffio.h>
#include <xtiffio.h>


unsigned char* tiff_load(const char *filename, int *width, int *height, int *channels) {

  TIFF *tif = XTIFFOpen(filename, "r");
  if (!tif) {
    printf("Could not open TIFF: %s", filename);
    return NULL;
  }

  int w, h, c;

  TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
  TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
  TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &c);

  *width = w;
  *height = h;
  *channels = c;

  //force rgba output
  int rgba_channels = 4;

  unsigned char *buf = (uint8_t*)malloc(w * h * rgba_channels);
  if (!buf) {
    printf("TIFF data memory allocation failed\n");
    XTIFFClose(tif);
    return NULL;
  }

  if (TIFFReadRGBAImage(tif, w, h, (uint32_t*)buf, 0)) {
    size_t row_size = w * rgba_channels;
    unsigned char *temp_row = (unsigned char*)malloc(row_size);
    for (size_t y = 0; y < (size_t)h/2; y++) {
      memcpy(temp_row, buf + y * row_size, row_size);
      memcpy(buf + y * row_size, buf + (h - y  -1) * row_size, row_size);
      memcpy(buf + (h - y - 1) * row_size, temp_row, row_size);
    }
    free(temp_row);
  } else {
    printf("TIFF data failed to be read as RGBA");
    free(buf);
    buf = NULL;
  }
  

  XTIFFClose(tif);
  return buf;
}

