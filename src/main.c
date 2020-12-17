#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <linmath/linmath.h>

static void
print_usage(const char* arg0)
{
    printf("usage: %s [options]\n", arg0);
    printf("\n");
    printf("Options:\n");
    printf("  -h --help        print this help\n");
}

static void
ppm_write_header(FILE* stream, long width, long height)
{
    fprintf(stream, "P3\n");
    fprintf(stream, "%ld %ld\n", width, height);
    fprintf(stream, "255\n");
}

static void
ppm_write_pixel(FILE* stream, float red, float green, float blue)
{
    unsigned char r = (unsigned char)(255 * red);
    unsigned char g = (unsigned char)(255 * green);
    unsigned char b = (unsigned char)(255 * blue);
    fprintf(stream, "%d %d %d\n", r, g, b);
}

int
main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    long width = 256;
    long height = 256;
    ppm_write_header(stdout, width, height);
    for (long y = height - 1; y >= 0; y--) {
        fprintf(stderr, "lines remaining: %ld\n", y);
        for (long x = 0; x < width; x++) {
            float r = (float)x / (width - 1);
            float g = (float)y / (height - 1);
            float b = 0.25f;
            ppm_write_pixel(stdout, r, g, b);
        }
    }

    return EXIT_SUCCESS;
}
