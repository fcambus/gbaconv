# GBAconv

## Description

GBAconv is a set of tools aimed at GameBoy Advance developers, allowing
to convert WAV sounds and PCX images into C arrays to be included in
GBA programs.

It is composed of two programs:

- **pcx2gba** : exports a PCX 8-bpp image to 16-bit values C arrays
           (palette + image data (2 pixels per value))
- **wav2gba** : exports a WAV 8-bit mono sound to 8-bit signed values array

There won't be any other releases of GBAconv, as I've no interest for the GBA platform anymore.

## Compiling

Simply type `make` to compile the programs.

They have been successfully compiled on:

- 32-bit x86 Architecture (Ubuntu Linux 5.10, FreeBSD 5.4)

## Usage

	pcx2gba input.pcx output.inc array_name (Input File must be 8-bit PCX)
	wav2gba input.wav output.inc array_name (Input File must be 8-bit, MONO)

## License

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

## Author

GBAconv is developed by Frederic Cambus

- Site : http://www.cambus.net
- Twitter: http://twitter.com/fcambus

## Resources

Project Homepage : http://fcambus.github.io/gbaconv/