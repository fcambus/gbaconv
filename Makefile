###############################################################################
#                                                                             #
# GBAconv 1.00                                                                #
# Copyright (c) 2002-2016, Frederic Cambus                                    #
# https://github.com/fcambus/gbaconv                                          #
#                                                                             # 
# Last Updated: 2016-01-05                                                    #
#                                                                             # 
# GBAconv is released under the BSD 3-Clause license.                         #
# See LICENSE file for details.                                               #
#                                                                             # 
###############################################################################


CC = gcc
TARGET = all
CFLAGS = -Wall -ansi -pedantic 

all :
	$(CC) $(CFLAGS) -o pcx2gba pcx2gba.c
	$(CC) $(CFLAGS) -o wav2gba wav2gba.c

clean:
	rm -f pcx2gba wav2gba *.o *.core core
