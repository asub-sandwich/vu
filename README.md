<h1 align="center">vu</h1>

<h4 align="center">A minimal image viewer built with SDL2 and stb_image.</h4>

## Key Features

* Loads those images!
* Zoom and pan, with ui zoom buttons
* Thats it!

## Usage

```bash
vu <image_path>
```

## Required Dependencies

  * SDL2
  * meson
  * ninja

## Installation

```bash
$ git clone https://github.com/asub-sandwich/vu
$ cd vu
$ meson setup build
$ meson compile -C build
$ meson install -C build
```

## Known Issues

  + Stretching can occur on zoom with odd window sizes

  + Image texture can disappear offscreen on zoom

  + My Zoom logic is just boned. 

## Plans

I would like to add tif and geotiff compatibility, where 1 band images can have some predefined color ramps passed as an option. Possibly also pass an option to choose bands for false color visualization of multispectral images. I would also like to render markdown and html, and display PDFs.

Basically, I want one tool, that just view the thing, hopefully with very little overhead.

## License

MIT

---
