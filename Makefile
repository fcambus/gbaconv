###############################################################################
#                                                                             #
# GBAconv 1.00 (c) by Frederic Cambus 2002-2006                               #
# http://gbaconv.sourceforge.net                                              #
#                                                                             #
# Last Updated: 2006/01/12                                                    #
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
