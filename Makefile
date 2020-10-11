#
# GBAconv 1.00
# Copyright (c) 2002-2019, Frederic Cambus
# https://github.com/fcambus/gbaconv
#
# Last Updated: 2019-02-21
#
# GBAconv is released under the BSD 2-Clause license.
# See LICENSE file for details.
#

CC ?= cc
CFLAGS += -D_FORTIFY_SOURCE=2 -O2 -std=c99 -Wall -pedantic

all:
	$(CC) $(CFLAGS) -o pcx2gba pcx2gba.c
	$(CC) $(CFLAGS) -o wav2gba wav2gba.c

clean:
	rm -f pcx2gba wav2gba *.o
