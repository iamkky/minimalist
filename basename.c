#include <getopt.h>
#include <libgen.h>
#include <stdio.h>
#include <string.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: basename [-az] NAME... [-s SUFFIX]\n"
		"	   basename NAME [SUFFIX]\n"
		"\n"
		"Output the final component of each pathname.\n"
		"\n"
		"  -a, --multiple	   Support multiple pathnames\n"
		"  -s, --suffix=SUFFIX  Remove a trailing suffix (implies -a)\n"
		"  -z, --zero		   End each output with NUL instead of newline\n"
		"  -h, --help		   Show this help\n");
}

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"multiple", no_argument, NULL, 'a'},
		{"suffix", required_argument, NULL, 's'},
		{"zero", no_argument, NULL, 'z'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	const char *suffix = NULL;
	int option;
	int multiple = 0;
	int zero = 0;

	opterr = 0;

	while ((option = getopt_long(argc, argv, "as:zh", long_options, NULL)) != -1) {
		switch (option) {
			case 'a':
				multiple = 1;
				break;
			case 's':
				suffix = optarg;
				multiple = 1;
				break;
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

	int count = argc - optind;

	if (!count || (!multiple && count > 2)) {
		usage(1);
		return 1;
	}

	if (!multiple && count == 2) {
		suffix = argv[optind + 1];
		count = 1;
	}

	for (int i = 0; i < count; i++) {
		char *name = basename(argv[optind + i]);
		size_t length = strlen(name);

		if (suffix && *suffix) {
			size_t suffix_length = strlen(suffix);

			if (length > suffix_length &&
				!strcmp(name + length - suffix_length, suffix))
				length -= suffix_length;
		}

		fwrite(name, 1, length, stdout);
		putchar(zero ? '\0' : '\n');
	}

	return ferror(stdout) ? 1 : 0;
}
