#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: link FILE1 FILE2\n"
		"\n"
		"Create a hard link named FILE2 to the existing FILE1.\n"
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

	if (argc - optind != 2) {
		usage(1);
		return 1;
	}

	const char *source = argv[optind];
	const char *destination = argv[optind + 1];

	if (link(source, destination) != 0) {
		fprintf(stderr, "link: cannot create link '%s' to '%s': %s\n",
				destination, source, strerror(errno));
		return 1;
	}

	return 0;
}
