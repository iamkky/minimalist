#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: hostid\n"
		"\n"
		"Output the numeric identifier of the current host in hexadecimal.\n"
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

	while ((option = getopt_long(argc, argv, "h", long_options, NULL)) != -1) {
		switch (option) {
			case 'h':
				usage(0);
				return 0;
			default:
				usage(1);
				return 1;
		}
	}

	if (optind != argc) {
		usage(1);
		return 1;
	}

	printf("%08lx\n", gethostid() & 0xffffffffUL);

	return ferror(stdout) ? 1 : 0;
}
