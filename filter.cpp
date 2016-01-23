/*
 * filter.cpp
 *
 *  Created on: Jan 23, 2016
 *      Author: mxx
 */

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "jfif_filter.h"

void usage(const char *name, FILE *out) {
	fprintf(out, "usage: %s <file1.jpg> \n", name);
	fprintf(out,
			"  filter the no standard marks structure from a JPEG stream. output to stdout. you can redirect the output to any file.\n");
	return;
}

std::string read_stream(FILE* in) {
	std::string str;
	while (!feof(in))
		str.append(1,fgetc(in));
	return str;
}

int main(int argc, char *argv[]) {

	FILE *in;

	if (argc < 2) {
		usage(argv[0], stderr);
		return 0;
	}

	/* treat the filename "-" as stdin */
	if (argv[1][0] == '-' && argv[1][1] == 0)
		in = stdin;

	const char *fn = argv[1];
	if (fn[0] == '-' && fn[1] == 0)
		in = stdin;
	else
		in = fopen(fn, "rb");
	if (in != NULL) {
		/* do something */
		std::cout << filter_stream(read_stream(in));
		if (in != stdin)
			fclose(in);
	} else {
		fprintf(stderr, "could not open '%s'\n", argv[1]);
		return 1;
	}

	return 0;
}
