#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <unistd.h>

#include "transfo.h"
#ifdef USE_CLOCK
#include <time.h>
#else
#include "cycles.h"
#endif

void read_look_up_table(FILE *map, unsigned char *lut)
{
	if (fread(lut, 1, 256, map) != 256) {
        perror("Error reading look-up table");
        exit(EXIT_FAILURE);
    }
}

void read_image(FILE *in, unsigned char *source_image, long size)
{
	fseek(in, 1, SEEK_CUR);
    if (fread(source_image, 1, size, in) != size) {
        perror("fread");
        exit(EXIT_FAILURE);
    }
}

void write_image(char *dest, unsigned char *img, long size, int width, int height, int maxval)
{	FILE *out = fopen(dest, "w");
    if (out == NULL) {
        perror(dest);
        exit(EXIT_FAILURE);
    }
    fprintf(out, "P5\n%d %d\n%d\n", height, width, maxval);
    if (fwrite(img, 1, size, out) != size) {
        perror("fwrite");
        exit(EXIT_FAILURE);
    }
    fclose(out);
}

#ifdef USE_CLOCK
clock_t transform_image(char *source, char *curve, int light, char *dest)
#else
double transform_image(char *source, char *curve, int light, char *dest)
#endif	
{
	FILE *in;
	FILE *map;
	char c1;
	char c2;
	int height;
	int width;
	int maxval;
	long size;
	unsigned char *source_image;
	unsigned char *dest_image;
	unsigned char *lut;
#ifdef USE_CLOCK
	clock_t start;
	clock_t stop;
	clock_t t;
#else
	double t;
#endif

	in = fopen(source, "r");
	if (in == NULL) {
		perror(source);
		exit(EXIT_FAILURE);
	}

	map = fopen(curve, "r");
	if (map == NULL) {
		perror(curve);
		exit(EXIT_FAILURE);
	}

	fscanf(in, "%c %c", &c1, &c2);
	if (c1 != 'P' || c2 != '5') {
		fprintf(stderr, "Error, input file is not PGM\n");
		exit(EXIT_FAILURE);
	}

	fscanf (in, "%d %d %d", &height, &width, &maxval);

	if (maxval > 255) {
		fprintf(stderr, "Input file is not a 8 bits gray image.\n");
		exit(EXIT_FAILURE);
	}

	size = width * height;
	printf ("%s: %d x %d = %ld pixels\n", source, width, height, size);

	/*
	 * Allocate memory for images and look up table
	 */
	source_image = (unsigned char *)malloc(sizeof(unsigned char) * size);
	if (source_image == NULL) {
		perror ("malloc for source image");
		exit (1);
	}
	dest_image = (unsigned char *)malloc(sizeof(unsigned char) * size);
	if (dest_image == NULL) {
		perror ("malloc for destination image");
		exit (1);
	}
	lut = (unsigned char *)malloc(sizeof(unsigned char) * 256);
	if (lut == NULL) {
		perror ("malloc for lut");
		exit (1);
	}

	read_look_up_table(map, lut);

	read_image(in, source_image, size);
	fclose (in);

#ifdef USE_CLOCK
	start = clock();
#else
	start_counter();
#endif
	transfo (width, height, source_image, lut, light);
#ifdef USE_CLOCK
	stop = clock();
	t = stop - start;
	printf("%ld clock cycles.\n", t);
#else
	t = get_counter();
	printf("%f clock cycles.\n", t);
#endif
	write_image(dest, source_image, size, width, height, maxval);

	free(source_image);
	free(lut);

	return t;
}

#define FNMAX 255

void run_transfo_file(FILE *tf)
{
	char source[FNMAX];
	char curve[FNMAX];
	char dest[FNMAX];
	int light;
#ifdef USE_CLOCK
	clock_t total = 0;
#else
	double total = 0;
#endif
	while (fscanf(tf, "%s %s %d %s", source, curve, &light, dest) == 4) {
		printf("%s %s %d %s\n", source, curve, light, dest);
		total += transform_image(source, curve, light, dest);
	}

#ifdef USE_CLOCK
	printf("TOTAL: %ld clock cycles.\n", total);
#else
	printf("TOTAL: %f clock cycles.\n", total);
#endif
}

int main (int ac, char *av[])
{
	FILE *tf;
	char *dname;

	if (ac != 2) {
		printf("Usage: %s transfofile\n", av[0]);
		exit(1);
	}

	tf = fopen(av[1], "r");
	if (tf == NULL) {
		perror(av[1]);
		exit(EXIT_FAILURE);
	}

	dname = dirname(av[1]);

	if (chdir(dname) == -1) {
		perror(dname);
		exit(EXIT_FAILURE);
	}

	run_transfo_file(tf);

	fclose(tf);

	return 0;
}
