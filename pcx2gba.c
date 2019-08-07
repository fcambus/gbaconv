/*
 * GBAconv 1.00
 * Copyright (c) 2002-2019, Frederic Cambus
 * https://github.com/fcambus/gbaconv
 *
 * PCX to GBA Converter
 *
 * Created:      2002-12-09
 * Last Updated: 2019-07-28
 *
 * GBAconv is released under the BSD 2-Clause license.
 * See LICENSE file for details.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PCX_HEADER_LENGTH 128
#define PCX_PALETTE_LENGTH 768

unsigned char *input_file_buffer;

struct pcx_header {
	uint8_t ID;
	uint8_t version;
	uint8_t encoding;
	uint8_t bits_per_pixel;
	uint16_t x_min;
	uint16_t y_min;
	uint16_t x_max;
	uint16_t y_max;
	uint16_t x_resolution;
	uint16_t y_resolution;
	uint8_t palette[48];
	uint8_t reserved;
	uint8_t number_of_bit_planes;
	uint16_t bytes_per_line;
	uint16_t palette_type;
	uint16_t x_screen_size;
	uint16_t y_screen_size;
	uint8_t filler[54];
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
		return EXIT_SUCCESS;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return EXIT_FAILURE;

	if (fstat(fd, &st) == -1) {
		close(fd);
		return EXIT_FAILURE;
	}

	if (st.st_size < PCX_HEADER_LENGTH + PCX_PALETTE_LENGTH) {
		printf("ERROR: Input File is not a PCX file\n\n");
		return EXIT_FAILURE;
	}

	/* mmap input file into memory */
	input_file_buffer = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (input_file_buffer == MAP_FAILED) {
		close(fd);
		return EXIT_FAILURE;
	}

	memcpy(&pcx_header, input_file_buffer, PCX_HEADER_LENGTH);

	/* Check that the file is a valid PCX file */
	if (pcx_header.ID != 10) {
		printf("ERROR: Input File is not a PCX file\n\n");
		return EXIT_FAILURE;
	}

	/* Check that the file is a 8-bpp PCX */
	if (pcx_header.bits_per_pixel != 8) {
		printf("ERROR: Input File is not 8-bpp\n\n");
		return EXIT_FAILURE;
	}

	pcx_header.x_max++;
	pcx_header.y_max++;

	if ((pcx_header.x_max) > 4096 || (pcx_header.y_max) > 4096) {
		printf("ERROR: Input File height or width is larger than 4096\n\n");
		return EXIT_FAILURE;
	}

	/* Uncompress RLE encoded PCX Input File */
	pcx_buffer_size = pcx_header.x_max * pcx_header.y_max;
	pcx_buffer = malloc(pcx_buffer_size);

	loop = PCX_HEADER_LENGTH;

	while (loop < st.st_size - PCX_PALETTE_LENGTH) {
		current_byte = input_file_buffer[loop];

		if (current_byte > 192) {
			run_count = current_byte-192;

			for (run_position = 0; run_position < run_count; run_position++) {
				if (offset + run_position < pcx_buffer_size) {
					pcx_buffer[offset + run_position] = input_file_buffer[loop +1];
				} else {
					printf("ERROR: Input File is corrupt\n\n");
					return EXIT_FAILURE;
				}
			}
			offset += run_count;
			loop += 2;
		} else {
			if (offset < pcx_buffer_size) {
				pcx_buffer[offset] = current_byte;
			} else {
				printf("ERROR: Input File is corrupt\n\n");
				return EXIT_FAILURE;
			}

			offset++;
			loop++;
		}
	}

	for (loop = 0; loop < PCX_PALETTE_LENGTH; loop++) {
		pcx_image_palette[loop] = input_file_buffer[st.st_size - PCX_PALETTE_LENGTH + loop] / 8;
	}

	fprintf(stderr, "INPUT  FILE: %s (%ix%ix%i-bpp)\n", argv[1], pcx_header.x_max, pcx_header.y_max, pcx_header.bits_per_pixel);

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

	return EXIT_SUCCESS;
}
