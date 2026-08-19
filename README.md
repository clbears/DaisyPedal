# GuitarPedal

Daisy Seed firmware for a set of guitar pedal effects, built with
[libDaisy](https://github.com/electro-smith/libDaisy) and
[DaisySP](https://github.com/electro-smith/DaisySP).

## Effects

- `seed/Delay`
- `seed/GuitarReverb`
- `seed/TapeChorus`
- `seed/Tone`
- `seed/Tremolo`

## Setup

```sh
git clone --recursive <this-repo-url>
# or, if already cloned without --recursive:
git submodule update --init --recursive
```

Requires the `arm-none-eabi-gcc` toolchain on `$PATH`.

Build the libraries first (required once, and again after any submodule update):

```sh
cd libDaisy && make -j && cd ..
cd DaisySP && make -j && cd ..
```

## Building an effect

```sh
cd seed/Delay
make
```

## Flashing to hardware

```sh
make program-dfu   # via USB, after entering bootloader mode
make program        # via JTAG/SWD (e.g. STLink)
```
