#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define N USHRT_MAX
//#define OUT_BUFF_SIZE 10000

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s input_file\n", argv[0]);
		return 1;
	}

	char *in_path = argv[1];
	FILE *input = fopen(in_path, "rb");
	if (!input) {
		perror("Can't open input file");
		return 1;
	}
	
	long long i, tbl[N];
	memset(tbl, 0, N * sizeof(long long));
	unsigned short buf;
	while (fread(&buf, sizeof(unsigned short), 1, input)) tbl[buf]++;
	fclose(input);

	char *out_path = (char*)malloc(sizeof(char) * (strlen(in_path) + 8));
	if (!out_path) {
		perror("Can't allocate bytes");
		return 1;
	}
	snprintf(out_path, sizeof(char) * (strlen(in_path) + 8), "%s.sorted", in_path);
	puts(out_path);

	FILE *output = fopen(out_path, "wb");
	free(out_path);
	if (!output) {
		perror("Can't open output file");
		fclose(input);
		return 1;
	}

	puts("sorted");
	
	for (buf = 0; buf < N; ++buf) {
		for (i = 0; i < tbl[buf]; ++i) {
			fwrite(&buf, sizeof(unsigned short), 1, output);
//			if (i % OUT_BUFF_SIZE == 0)
//				fflush(output);
		}
	}
	puts("output success");

	fclose(output);
	return 0;
}
