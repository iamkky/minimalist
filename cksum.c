#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
Adapted from toybox (to standalone cmd)
https://codeberg.org/landley/toybox/src/branch/master/toys/posix/cksum.c
Originally licensed as:

Licensed as:
Copyright (C) 2006, 2019 by Rob Landley <rob@landley.net>

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

enum {
	FLAG_HEX = 1,
	FLAG_NO_INVERT = 2,
	FLAG_PRE_INVERT = 4,
	FLAG_LITTLE_ENDIAN = 8,
	FLAG_NO_LENGTH = 16
};

static void usage(const char *program, int status)
{
	FILE *output = status ? stderr : stdout;

	if (!strcmp(program, "crc32")) {
		fprintf(output,
			"Usage: crc32 [FILE...]\n"
			"\n"
			"Output CRC32 checksum for each file.\n");
	} else {
		fprintf(output,
			"Usage: cksum [-HIPLN] [FILE...]\n"
			"\n"
			"Output checksum, length and filename for each file.\n"
			"Use standard input if no files are specified or FILE is '-'.\n"
			"\n"
			"  -H    Hexadecimal checksum (default: decimal)\n"
			"  -I    Skip post-inversion\n"
			"  -P    Pre-inversion\n"
			"  -L    Little endian (default: big endian)\n"
			"  -N    Do not include length in CRC calculation or output\n");
	}

	fprintf(output, "  -h, --help    Show this help\n");
}

static void crc_init(uint32_t table[256], int little_endian)
{
	for (unsigned i = 0; i < 256; i++) {
		uint32_t crc = little_endian ? i : i << 24;

		for (int j = 0; j < 8; j++) {
			if (little_endian)
				crc = (crc & 1) ? (crc >> 1) ^ 0xedb88320 : crc >> 1;
			else
				crc = (crc & 0x80000000) ? (crc << 1) ^ 0x04c11db7 : crc << 1;
		}

		table[i] = crc;
	}
}

static int do_cksum(int fd, const char *name, int flags, int show_name)
{
	uint32_t table[256];
	uint32_t crc = (flags & FLAG_PRE_INVERT) ? UINT32_MAX : 0;
	unsigned long long length = 0;
	unsigned char buffer[4096];
	ssize_t count;

	crc_init(table, flags & FLAG_LITTLE_ENDIAN);

	while ((count = read(fd, buffer, sizeof(buffer))) > 0) {
		length += count;

		for (ssize_t i = 0; i < count; i++)
			crc = (flags & FLAG_LITTLE_ENDIAN)
				? table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8)
				: (crc << 8) ^ table[((crc >> 24) ^ buffer[i]) & 0xff];
	}

	if (count < 0) {
		fprintf(stderr, "%s: %s\n", name, strerror(errno));
		return 1;
	}

	if (!(flags & FLAG_NO_LENGTH)) {
		unsigned long long remaining = length;

		while (remaining) {
			unsigned char byte = remaining & 0xff;

			crc = (flags & FLAG_LITTLE_ENDIAN)
				? table[(crc ^ byte) & 0xff] ^ (crc >> 8)
				: (crc << 8) ^ table[((crc >> 24) ^ byte) & 0xff];

			remaining >>= 8;
		}
	}

	if (!(flags & FLAG_NO_INVERT))
		crc = ~crc;

	if (flags & FLAG_HEX)
		printf("%08x", crc);
	else
		printf("%u", crc);

	if (!(flags & FLAG_NO_LENGTH))
		printf(" %llu", length);

	if (show_name)
		printf(" %s", name);

	putchar('\n');

	return 0;
}

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int flags = 0;
	int option;
	int status = 0;

	const char *program = strrchr(argv[0], '/');
	program = program ? program + 1 : argv[0];

	int is_crc32 = !strcmp(program, "crc32");

	if (is_crc32)
		flags = FLAG_HEX | FLAG_PRE_INVERT |
			FLAG_LITTLE_ENDIAN | FLAG_NO_LENGTH;

	opterr = 0;

	while ((option = getopt_long(argc, argv,
			is_crc32 ? "+h" : "+HIPLNh",
			long_options, NULL)) != -1) {
		switch (option) {
			case 'H':
				flags |= FLAG_HEX;
				break;
			case 'I':
				flags |= FLAG_NO_INVERT;
				break;
			case 'P':
				flags |= FLAG_PRE_INVERT;
				break;
			case 'L':
				flags |= FLAG_LITTLE_ENDIAN;
				break;
			case 'N':
				flags |= FLAG_NO_LENGTH;
				break;
			case 'h':
				usage(program, 0);
				return 0;
			default:
				usage(program, 1);
				return 1;
		}
	}

	if (optind == argc)
		return do_cksum(STDIN_FILENO, "-", flags, 0);

	for (int i = optind; i < argc; i++) {
		const char *name = argv[i];
		int fd = !strcmp(name, "-") ? STDIN_FILENO : open(name, O_RDONLY);

		if (fd < 0) {
			fprintf(stderr, "%s: %s\n", name, strerror(errno));
			status = 1;
			continue;
		}

		status |= do_cksum(fd, name, flags, 1);

		if (fd != STDIN_FILENO && close(fd) < 0) {
			fprintf(stderr, "%s: %s\n", name, strerror(errno));
			status = 1;
		}
	}

	if (fflush(stdout) == EOF)
		status = 1;

	return status;
}

