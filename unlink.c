#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: unlink FILE\n"
		"\n"
		"Remove the specified file using the unlink system call.\n"
		"\n"
		"  -h, --help	Show this help\n");
}

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int option;

	opterr = 0;

	while ((option = getopt_long(argc, argv, "+h", long_options, NULL)) != -1) {
		switch (option) {
			case 'h':
				usage(0);
				return 0;
			default:
				usage(1);
				return 1;
		}
	}

	if (argc - optind != 1) {
		usage(1);
		return 1;
	}

	const char *filename = argv[optind];

	if (unlink(filename) != 0) {
		fprintf(stderr, "unlink: cannot unlink '%s': %s\n",
				filename, strerror(errno));
		return 1;
	}

	return 0;
}
