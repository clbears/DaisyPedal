# GuitarPedal

Daisy Seed firmware for a set of guitar pedal effects, built with
[libDaisy](https://github.com/electro-smith/libDaisy) and
[DaisySP](https://github.com/electro-smith/DaisySP).

Each effect is a standalone firmware image (its own `main()`) — pick one,
build it, and flash it to the Seed. There's no menu or preset switching
between effects; swapping effects means reflashing.

## Effects

| Effect | Description | Pot (pin 22 / A0) controls |
| --- | --- | --- |
| [`seed/Delay`](seed/Delay) | Fixed-length (~0.9s) echo | Feedback / number of repeats |
| [`seed/GuitarReverb`](seed/GuitarReverb) | Algorithmic stereo-collapsed reverb | Decay time |
| [`seed/TapeChorus`](seed/TapeChorus) | Chorus with tape-style wow/flutter drift and soft saturation | Drift depth ("how worn out the tape is") |
| [`seed/Tone`](seed/Tone) | *WIP — currently a duplicate of GuitarReverb* | — |
| [`seed/Tremolo`](seed/Tremolo) | Amplitude tremolo | Rate (~0.5-10 Hz) |

All effects expect guitar input on **AUDIO IN R** (pin 17) and mirror their
output to both output channels.

## Roadmap: all-in-one pedal

The current firmware images are single-effect proofs of concept, each using
one pot. The end goal is a single unit with an 8-pot control surface and a
mode dial that switches between Chorus, Vibrato, Delay, and Reverb engines.

Planned potentiometer mapping:

| Pot | Chorus | Vibrato | Delay | Reverb |
| --- | --- | --- | --- | --- |
| 1 (global) | FX engine selector dial | | | |
| 2 (global) | Independent wet mix / level balance | | | |
| 3 | Rate / speed | Rate / speed | Delay time | Decay / size (RT₆₀) |
| 4 | Dependent depth *(drops at high rates)* | Depth | Smart feedback *(prevents harsh spikes)* | High-frequency damp |
| 5 | Tone shifter | LFO shape morph *(sine → tri → square)* | Lo-fi bandpass filter *(cuts highs and lows)* | Pre-delay time |
| 6 | Delay baseline offset | Stereo phase split | Modulation rate | Reflection diffusion |
| 7 | Tape cassette drift *(random pitch micro-slips)* | Stereo width | Modulation depth | Dynamic envelope duck *(ducks mix on hard attack)* |
| 8 | Resonant feedback *(pushes into flanger)* | Output volume trim | Wet-path saturation *(overdrives only echoes)* | Ambient drone morph *(changes tail past 50%)* |

Pots 1 and 2 are global (same function regardless of mode); pots 3-8 are
dynamic and are remapped per-mode by firmware.

## Setup

```sh
git clone --recursive https://github.com/clbears/DaisyPedal.git
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

## License

Firmware in `seed/` has no separate license file; treat it as
all-rights-reserved unless the author says otherwise. `libDaisy` and
`DaisySP` are vendored as git submodules and are each MIT-licensed by
Electrosmith — see their respective `LICENSE` files.
