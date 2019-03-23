/*
 * GBAconv 1.00
 * Copyright (c) 2002-2017, Frederic Cambus
 * https://github.com/fcambus/gbaconv
 *
 * PCX to GBA Converter
 *
 * Created:      2002-12-09
 * Last Updated: 2019-02-22
 *
 * GBAconv is released under the BSD 2-Clause license.
 * See LICENSE file for details.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PCX_HEADER_LENGTH 128
#define PCX_PALETTE_LENGTH 768

unsigned char *input_file_buffer;

struct pcx_header {
	char ID;
	char version;
	char encoding;
	char bits_per_pixel;
	short int x_min;
	short int y_min;
	short int x_max;
	short int y_max;
	short x_resolution;
	short y_resolution;
	char palette[48];
	char reserved;
	char number_of_bit_planes;
	short bytes_per_line;
	short palette_type;
	short x_screen_size;
	short y_screen_size;
	char filler[54];
} pcx_header;

unsigned char pcx_image_palette[PCX_PALETTE_LENGTH];
unsigned char *pcx_buffer;
int pcx_buffer_size;

int loop;

int current_byte;
int offset;
int run_count;
int run_position;

int main(int argc, char *argv[]) {
	int fd;
	struct stat st;

	if (argc != 3) {
		printf("USAGE: pcx2gba input.pcx array_name (Input File must be 8-bpp PCX)\n\n");
		return 0;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return 1;

	if (fstat(fd, &st) == -1) {
		close(fd);
		return 1;
	}

	if (st.st_size < PCX_HEADER_LENGTH + PCX_PALETTE_LENGTH) {
		printf("ERROR: Input File is not a PCX file\n\n");
		return 1;
	}

	/* mmap input file into memory */
	input_file_buffer = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (input_file_buffer == MAP_FAILED) {
		close(fd);
		return 1;
	}

	memcpy(&pcx_header, input_file_buffer, PCX_HEADER_LENGTH);

	/* Check that the file is a valid PCX file */
	if (pcx_header.ID != 10) {
		printf("ERROR: Input File is not a PCX file\n\n");
		return 1;
	}

	/* Check that the file is a 8-bpp PCX */
	if (pcx_header.bits_per_pixel != 8) {
		printf("ERROR: Input File is not 8-bpp\n\n");
		return 1;
	}

	/* Uncompress RLE encoded PCX Input File */
	pcx_buffer_size = (pcx_header.x_max+1)*(pcx_header.y_max+1);
	pcx_buffer = malloc(pcx_buffer_size);

	loop = PCX_HEADER_LENGTH;

	while (loop < st.st_size - PCX_PALETTE_LENGTH) {
		current_byte = input_file_buffer[loop];

		if (current_byte > 192) {
			run_count = current_byte-192;

			for (run_position = 0; run_position < run_count; run_position++) {
				pcx_buffer[offset+run_position] = input_file_buffer[loop+1];
			}
			offset += run_count;
			loop += 2;
		} else {
			pcx_buffer[offset] = current_byte;
			offset++;
			loop++;
		}
	}

	for (loop = 0; loop < PCX_PALETTE_LENGTH; loop++) {
		pcx_image_palette[loop] = input_file_buffer[st.st_size - PCX_PALETTE_LENGTH + loop] / 8;
	}

	fprintf(stderr, "INPUT  FILE: %s (%ix%ix%i-bpp)\n", argv[1], pcx_header.x_max+1, pcx_header.y_max+1, pcx_header.bits_per_pixel);

	fprintf(stdout, "const u16 %s_palette[] = {\n", argv[2]);

	for (loop = 0; loop < 256; loop++) {
		fprintf(stdout, "0x%x,", pcx_image_palette[loop*3] | pcx_image_palette[(loop*3)+1]<<5 | pcx_image_palette[(loop*3)+2]<<10);
	}

	fprintf(stdout, "};\n\n");

	fprintf(stdout, "const u16 %s[] = {\n", argv[2]);

	for (loop = 0; loop < pcx_buffer_size/2; loop++) {
		fprintf(stdout, "0x%x,", pcx_buffer[loop*2] | pcx_buffer[(loop*2)+1]<<8);
	}

	fprintf(stdout, "};\n");

	/* Terminate Program */
	munmap(input_file_buffer, st.st_size);
	close(fd);

	return 0;
}
