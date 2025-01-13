#include <omp.h>  // Include OpenMP header for parallelization

void copy (int w, int h, unsigned char *src, unsigned char *dest)
{
	int i,j;

  	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++) {
			dest[j * w + i] = src[j * w + i];
		}
	}
}

void light(int w, int h, unsigned char *img, unsigned char val)
{
	int i,j;
	unsigned char current;

	for (i = 0; i < w; i++) {
		for (j = 0; j < h; j++) {
			current = img[j * w + i];
			img[j * w + i] = (((int) current + val) > 255) ? 255 : current + val;
		}
	}
}

void curve(int w, int h, unsigned char *img, unsigned char *lut)
{
	int i,j;
  	unsigned char current;

  	for (i = 0; i < w; i++) {
  		for (j = 0; j < h; j++) {
  			current = img[j * w + i];
			img[j * w + i] = lut[current];
  		}
  	}
}

void transfo(int w, int h, unsigned char *src, unsigned char *lut, unsigned char val)
{
  	// copy(w, h, src, dest);
  	// curve(w, h, src, lut);
  	// light(w, h, src, val);

	int i,j;
	int size = w * h;

    // Parallelize the loop using OpenMP
    #pragma omp parallel for num_threads(omp_get_max_threads()) private(i) shared(src, lut, val)
    for (i = 0; i < size; i++) {
        unsigned char current = lut[src[i]];
        src[i] = (((int) current + val) > 255) ? 255 : current + val;
    }
}
