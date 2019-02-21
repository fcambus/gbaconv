/*
 * GBAconv 1.00
 * Copyright (c) 2002-2017, Frederic Cambus
 * https://github.com/fcambus/gbaconv
 *
 * PCX to GBA Converter
 *
 * Created:      2002-12-09
 * Last Updated: 2019-02-21
 *
 * GBAconv is released under the BSD 2-Clause license.
 * See LICENSE file for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define PCX_HEADER_LENGTH 128

FILE *input_file;
unsigned char *input_file_buffer;
int input_file_size;
struct stat input_file_stat;

FILE *output_file;

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

unsigned char pcx_image_palette[768];
unsigned char *pcx_buffer;
int pcx_buffer_size;

int loop;

int current_byte;
int offset;
int run_count;
int run_position;

int main(int argc, char *argv[]) {
	if (argc != 4) {
		printf("USAGE: pcx2gba input.pcx output.inc array_name (Input File must be 8-bpp PCX)\n\n");
		return 0;
	}

	/* Load Input File */
	stat(argv[1], &input_file_stat);
	input_file_size = input_file_stat.st_size;

	input_file_buffer = malloc(input_file_size);

	if (input_file_buffer == NULL) {
		printf("ERROR: Cannot allocate memory\n\n");
		return -1;
	}

	input_file = fopen(argv[1], "rb");
	if (input_file == NULL) {
		printf("ERROR: Cannot open file %s\n\n", argv[1]);
		return -1;
	}

	fread(input_file_buffer, input_file_size, 1, input_file);
	fclose(input_file);

	if (input_file_size < PCX_HEADER_LENGTH) {
		printf("ERROR: Input File is not a PCX file\n\n");
		return -1;
	}

	/* Check that the file is a valid 8-bpp PCX */
	memcpy(&pcx_header, input_file_buffer, PCX_HEADER_LENGTH);

	if (pcx_header.bits_per_pixel != 8) {
		printf("ERROR: Input File is not 8-bpp\n\n");
		return -1;
	}

	/* Uncompress RLE encoded PCX Input File */
	pcx_buffer_size = (pcx_header.x_max+1)*(pcx_header.y_max+1);
	pcx_buffer = malloc(pcx_buffer_size);

	loop = PCX_HEADER_LENGTH;

	while (loop < input_file_size-768) {
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

	for (loop = 0; loop < 768; loop++) {
		pcx_image_palette[loop] = (input_file_buffer[input_file_size-768+loop]/8);
	}

	/* Create Output File */
	output_file = fopen(argv[2], "w");
	if (output_file == NULL) {
		printf("ERROR: Cannot create file %s\n\n", argv[2]);
		return -1;
	}

	printf("INPUT  FILE: %s (%ix%ix%i-bpp)\n", argv[1], pcx_header.x_max+1, pcx_header.y_max+1, pcx_header.bits_per_pixel);
	printf("OUTPUT FILE: %s\n\n", argv[2]);

	fprintf(output_file, "const u16 %s_palette[] = {\n", argv[3]);

	for (loop = 0; loop < 256; loop++) {
		fprintf(output_file, "0x%x,", (pcx_image_palette[loop*3] | pcx_image_palette[(loop*3)+1]<<5 | pcx_image_palette[(loop*3)+2]<<10));
	}

	fseek(output_file, ftell(output_file)-1, 0);
	fprintf(output_file, "};\n\n");

	fprintf(output_file, "const u16 %s[] = {\n", argv[3]);

	for (loop = 0; loop < pcx_buffer_size/2; loop++) {
		fprintf(output_file, "0x%x,", (pcx_buffer[loop*2] | pcx_buffer[(loop*2)+1]<<8));
	}

	fseek(output_file, ftell(output_file)-1, 0);
	fprintf(output_file, "};\n");

	/* Terminate Program */
	fclose(output_file);
	free(input_file_buffer);

	printf("Successfully created file %s\n\n", argv[2]);

	return 0;
}
