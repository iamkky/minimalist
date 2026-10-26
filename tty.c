#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: tty [-s]\n"
		"\n"
		"Output the filename of the terminal connected to standard input.\n"
		"\n"
		"  -s, --silent, --quiet	Print nothing, only return an exit status\n"
		"  -h, --help			  Show this help\n");
}

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"silent", no_argument, NULL, 's'},
		{"quiet", no_argument, NULL, 's'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int option;
	int silent = 0;

	opterr = 0;

	while ((option = getopt_long(argc, argv, "sh", long_options, NULL)) != -1) {
		switch (option) {
			case 's':
				silent = 1;
				break;
			case 'h':
				usage(0);
				return 0;
			default:
				usage(2);
				return 2;
		}
	}

	if (optind != argc) {
		usage(2);
		return 2;
	}

	char *name = ttyname(STDIN_FILENO);
	int status = isatty(STDIN_FILENO) ? 0 : 1;

	if (!silent) {
		puts(name ? name : "not a tty");

		if (fflush(stdout) == EOF)
			return 3;
	}

	return status;
}
