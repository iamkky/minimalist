#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: dirname [-z] NAME...\n"
		"\n"
		"Output the directory portion of each pathname.\n"
		"\n"
		"  -z, --zero	End each output with NUL instead of newline\n"
		"  -h, --help	Show this help\n");
}

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"zero", no_argument, NULL, 'z'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int option;
	int zero = 0;

	opterr = 0;

	while ((option = getopt_long(argc, argv, "zh", long_options, NULL)) != -1) {
		switch (option) {
			case 'z':
				zero = 1;
				break;
			case 'h':
				usage(0);
				return 0;
			default:
				usage(1);
				return 1;
		}
	}

	if (optind == argc) {
		usage(1);
		return 1;
	}

	for (int i = optind; i < argc; i++) {
		fputs(dirname(argv[i]), stdout);
		putchar(zero ? '\0' : '\n');
	}

	return ferror(stdout) ? 1 : 0;
}

