# GBAconv

## Description

GBAconv is a set of tools aimed at GameBoy Advance developers, allowing
to convert WAV sounds and PCX images into C arrays to be included in
GBA programs.

It is composed of two programs:

- **pcx2gba**: exports a PCX 8-bpp image to 16-bit values C arrays
           (palette + image data (2 pixels per value))
- **wav2gba**: exports a WAV 8-bit mono sound to 8-bit signed values array

There won't be any other releases of GBAconv, as I've no interest for the GBA platform anymore.

## Compiling

Simply type `make` to compile the programs.

They have been successfully compiled on:

- 32-bit x86 Architecture (Ubuntu Linux 5.10, FreeBSD 5.4)

## Usage

	pcx2gba input.pcx output.inc array_name (Input File must be 8-bit PCX)
	wav2gba input.wav output.inc array_name (Input File must be 8-bit, MONO)

## License

GBAconv is released under the BSD 2-Clause license. See `LICENSE` file for details.

## Author

GBAconv is developed by Frederic Cambus.

- Site: https://www.cambus.net
