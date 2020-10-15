/*
 * GBAconv 1.00
 * Copyright (c) 2002-2019, Frederic Cambus
 * https://github.com/fcambus/gbaconv
 *
 * WAV to GBA Converter
 *
 * Created:      2002-12-10
 * Last Updated: 2019-05-09
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

#define WAVE_HEADER_LENGTH 44

char *input_file_buffer;

struct wave_header {
	char chunk_ID[4];
	unsigned int chunk_size;
	char format[4];
	char fmt_chunk[4];
	unsigned int fmt_chunk_size;
	unsigned short int audio_format;
	unsigned short int channels;
	unsigned int sample_rate;
	unsigned int byte_rate;
	unsigned short int block_align;
	unsigned short int bits_per_sample;
	char data_chunk[4];
	unsigned int data_chunk_size;
} __attribute__((packed)) wave_header;

int main(int argc, char *argv[])
{
	int fd;
	struct stat st;

	if (argc != 3) {
		printf("USAGE: wav2gba input.wav array_name (Input File must be 8-bit, MONO)\n\n");
		return EXIT_SUCCESS;
	}

	fd = open(argv[1], O_RDONLY);
	if (fd == -1)
		return EXIT_FAILURE;

	if (fstat(fd, &st) == -1) {
		close(fd);
		return EXIT_FAILURE;
	}

	/* mmap input file into memory */
	input_file_buffer = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (input_file_buffer == MAP_FAILED) {
		close(fd);
		return EXIT_FAILURE;
	}

	if (st.st_size < WAVE_HEADER_LENGTH) {
		printf("ERROR: Input File is not a WAV file\n\n");
		return EXIT_FAILURE;
	}

	/* Check that the file is a valid 8-bit MONO WAV */
	memcpy(&wave_header, input_file_buffer, WAVE_HEADER_LENGTH);

	if (wave_header.channels != 1) {
		printf("ERROR: Input File is not MONO\n\n");
		return EXIT_FAILURE;
	}

	if (wave_header.bits_per_sample != 8) {
		printf("ERROR: Input File is not 8-bit\n\n");
		return EXIT_FAILURE;
	}

	fprintf(stderr, "INPUT  FILE: %s (8-bit, MONO, %i Hz)\n", argv[1], wave_header.sample_rate);

	fprintf(stdout, "const s8 %s[] = {", argv[2]);

	for (size_t loop = 0; loop < st.st_size - WAVE_HEADER_LENGTH; loop++) {
		if (loop % 10 == 0)
			fprintf(stdout, "\n\t");

		fprintf(stdout, "0x%x,", input_file_buffer[WAVE_HEADER_LENGTH + loop] + 128);
	}

	fprintf(stdout, "\n};\n");

	/* Terminate Program */
	munmap(input_file_buffer, st.st_size);
	close(fd);

	return EXIT_SUCCESS;
}
