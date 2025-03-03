# vuer
A simple image viewer made with SDL and stb_image.

## Installing

### Install Dependencies

Debian based
`
  sudo apt install ninja meson libsdl2-dev
`
Fedora based
`
  sudo dnf install ninja meson SDL2-devel 
`
Arch based
`
  paru -S ninja meson sdl2
`

### Building with meson
`
  git clone https://github.com/asub-sandwich/vu && cd vu
  meson setup build
  meson compile -C build
`

### Installation
`
  meson install -C build
`
