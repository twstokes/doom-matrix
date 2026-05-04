# doom-matrix


https://github.com/twstokes/doom-matrix/assets/2092798/97c2c7a6-bf03-4654-8640-d9933a54d21e


_Note: Flickering isn't seen with human eyes. 👀_


Runs Doom on LED matrices connected to a Raspberry Pi.

**Made possible thanks to these libraries:**
- [doomgeneric](https://github.com/ozkl/doomgeneric)
- [rpi-rgb-led-matrix](https://github.com/hzeller/rpi-rgb-led-matrix)
- [SDL2](https://github.com/libsdl-org/SDL) / [SDL_mixer 2](https://github.com/libsdl-org/SDL_mixer)

For best performance it's recommended to run an OS like [DietPi](https://dietpi.com/).

## Hardware
- [64x64 LED Matrix](https://www.adafruit.com/product/4732)
- [Adafruit RGB Matrix Bonnet](https://www.adafruit.com/product/3211)
- [5V 10A Power Supply](https://www.adafruit.com/product/658)

## Setup tooling and libraries

1. Install C and C++ compilers + `make` for your OS
1. Fetch the dependency submodules with `git submodule update --init --recursive --depth=1`
### Audio

Audio should be mostly turn-key, but may need some extra setup on a Pi. It's not recommended to use the on-board audio while driving the matrix for performance reasons, so it's best to disable it on boot. A simple USB audio adapter configured as the default ALSA device is a better fit.

For music playback on Debian-based systems, these packages can still be useful:
- `libasound2-dev`
- `fluid-soundfont-gm`
- `freepats`
- `timidity`
- `fluidsynth`

**Note:** 
- Even if running `doom-matrix` as `root`, you'll probably need to add your user to the appropriate `audio` group.

If you want audio support, install:

`sudo apt install libsdl2-dev libsdl2-mixer-dev pkg-config`

## Building the project

`make`

This project is intended to be built on Linux, typically on a Raspberry Pi. The
`Makefile` will stop immediately on macOS because `rpi-rgb-led-matrix` depends
on Linux/Raspberry Pi userspace APIs.

Audio support is enabled by default. To build without audio, use:

```sh
make AUDIO=0
```

Audio-enabled builds use `SDL2_mixer` from the system. If audio is disabled,
the build skips `SDL_mixer` entirely.

### Cleaning

`make clean`

## Running

You need an IWAD file such as `doom1.wad` in your working directory, or you
can point Doom to it explicitly with `-iwad`.

The binary accepts arguments for both [rpi-rgb-led-matrix](https://github.com/hzeller/rpi-rgb-led-matrix/tree/master) and [doomgeneric](https://github.com/ozkl/doomgeneric), e.g.:

`./doom_matrix --led-gpio-mapping=adafruit-hat -iwad doom1.wad --led-rows=64 --led-cols=64`

Use the GPIO mapping and matrix geometry that match your hardware. See those
libraries for information on what arguments are available.
