#include <getopt.h>
#include <pwd.h>
#include <stdio.h>
#include <unistd.h>

static void usage(int status)
{
	FILE *output = status ? stderr : stdout;

	fprintf(output,
		"Usage: whoami\n"
		"\n"
		"Output the username associated with the effective user ID.\n"
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

	struct passwd *user = getpwuid(geteuid());

	if (!user) {
		fprintf(stderr, "whoami: cannot determine username\n");
		return 1;
	}

	puts(user->pw_name);

	return ferror(stdout) ? 1 : 0;
}

