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

Audio should be decently turn-key, but can take a little more work. It's not recommended to use the on-board audio on the Pi while driving the matrix for performance reasons, so it's best to disable it on boot. I used an inexpensive USB adapter and made it the default for ALSA.

Because I built the libraries locally I needed to install `libasound2-dev` beforehand and built [SDL](https://github.com/libsdl-org/SDL) and [SDL Mixer](https://github.com/libsdl-org/SDL_mixer) in the `libs/SDL` and `libs/SDL_mixer` paths respectively. [See build and installation instructions](https://wiki.libsdl.org/SDL2/Installation) for more info.

In `libs/SDL` and `libs/SDL_mixer` you should only have to run:
```
./configure
make
make install (as root)
```

**Note:** 
- It can take a while to compile these on a Pi! You can also consider cross-compiling from a faster machine.
- If your distro has SDL packages you can install them instead.
- Even if running `doom-matrix` `root`, you'll probably need to add your user to the appropriate `audio` group.

## Building the project

`make`

### Cleaning

`make clean`

## Running

The binary accepts arguments for both [rpi-rgb-led-matrix](https://github.com/hzeller/rpi-rgb-led-matrix/tree/master) and [doomgeneric](https://github.com/ozkl/doomgeneric), e.g.:

`./doom_matrix --led-gpio-mapping=adafruit-hat -iwad doom1.wad --led-rows=64 --led-cols=64`

See those libraries for information on what arguments are available.
