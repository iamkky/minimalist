
#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	static const struct option long_options[] = {
		{"data", no_argument, NULL, 'd'},
		{"file-system", no_argument, NULL, 'f'},
		{"help", no_argument, NULL, 'h'},
		{NULL, 0, NULL, 0}
	};

	const char *usage =
		"Usage: sync [OPTION] [FILE...]\n"
		"\n"
		"Synchronize cached writes to persistent storage.\n"
		"If files are specified, synchronize those files or their filesystems.\n"
		"\n"
		"  -d, --data		   Sync file data without unnecessary metadata\n"
		"  -f, --file-system	Sync the filesystems containing the files\n"
		"  -h, --help		   Show this help\n";

	int option;
	int data = 0;
	int filesystem = 0;
	int status = 0;

	opterr = 0;

	while ((option = getopt_long(argc, argv, "dfh", long_options, NULL)) != -1) {
		switch (option) {
			case 'd':
				data = 1;
				break;
			case 'f':
				filesystem = 1;
				break;
			case 'h':
				fputs(usage, stdout);
				return 0;
			default:
				fprintf(stderr, "sync: unknown option\n");
				fputs(usage, stderr);
				return 1;
		}
	}

	if (data && filesystem) {
		fprintf(stderr, "sync: cannot specify both --data and --file-system\n");
		return 1;
	}

	if (data && optind == argc) {
		fprintf(stderr, "sync: --data needs at least one argument\n");
		return 1;
	}

	if (optind == argc) {
		sync();
		return 0;
	}

	for (int i = optind; i < argc; i++) {
		const char *filename = argv[i];
		int fd = open(filename, O_RDONLY | O_NONBLOCK);

		if (fd < 0)
			fd = open(filename, O_WRONLY | O_NONBLOCK);

		if (fd < 0) {
			fprintf(stderr, "sync: cannot open '%s': %s\n",
					filename, strerror(errno));
			status = 1;
			continue;
		}

		int result = filesystem ? syncfs(fd) :
					 data ? fdatasync(fd) : fsync(fd);

		if (result < 0) {
			fprintf(stderr, "sync: cannot sync '%s': %s\n",
					filename, strerror(errno));
			status = 1;
		}

		if (close(fd) < 0) {
			fprintf(stderr, "sync: cannot close '%s': %s\n",
					filename, strerror(errno));
			status = 1;
		}
	}

	return status;
}
