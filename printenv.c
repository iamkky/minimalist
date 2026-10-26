#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern char **environ;

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: printenv [-0] [VARIABLE...]\n"
		"\n"
		"Output the values of the specified environment variables.\n"
		"If no variables are specified, print the entire environment.\n"
		"\n"
		"  -0, --null	End each output with NUL instead of newline\n"
		"  -h, --help	Show this help\n");
}

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"null", no_argument, NULL, '0'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	int option;
	int zero = 0;
	int status = 0;

	opterr = 0;

	while ((option = getopt_long(argc, argv, "+0h", long_options, NULL)) != -1) {
		switch (option) {
			case '0':
				zero = 1;
				break;
			case 'h':
				usage(0);
				return 0;
			default:
				usage(2);
				return 2;
		}
	}

	if (optind == argc) {
		for (char **variable = environ; *variable; variable++) {
			fputs(*variable, stdout);
			putchar(zero ? '\0' : '\n');
		}
	} else {
		for (int i = optind; i < argc; i++) {
			if (strchr(argv[i], '=')) {
				status = 1;
				continue;
			}

			const char *value = getenv(argv[i]);

			if (value) {
				fputs(value, stdout);
				putchar(zero ? '\0' : '\n');
			} else {
				status = 1;
			}
		}
	}

	if (fflush(stdout) == EOF)
		return 2;

	return status;
}
