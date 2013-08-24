#include <tarfs.h>

#include <utils.h>

typedef struct tar_header {
	char filename[100];
	char mode[8];
	char uid[8];
	char gid[8];
	char size[12];
	char chksum[8];
	char typeflag[1];
} tar_header;

static unsigned from_8_base(const char *in, int len) {
	unsigned res = 0;
	for (int i = 0; i < len; ++i) {
		res = 8*res + in[i] - '0';
	}
	return res;
}

static unsigned round_up_to(unsigned a, unsigned round) {
	return ((a - 1)/round + 1)*round;
}

void *tar_get_file(void *base,  const char *filename) {
	tar_header *head = (tar_header *)base;
	while (head->filename[0]) {
		if (strcmp(head->filename, filename) == 0) {
			return (void *)((unsigned)head + 512);
		}

		unsigned file_size = from_8_base(head->size, 11);
		head = (tar_header *)((unsigned)head + 512 +
			round_up_to(file_size, 512));
	}
	return 0;
}
