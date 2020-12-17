#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
ppm_write_pixel(FILE* stream, float r, float g, float b)
{
    fprintf(stream, "%d %d %d\n",
        (unsigned char)(r * 255),
        (unsigned char)(g * 255),
        (unsigned char)(b * 255));
}

struct vec3 {
    float x, y, z;
};

static struct vec3
vec3_add(struct vec3 v1, struct vec3 v2)
{
    struct vec3 v = {
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
        .z = v1.z + v2.z,
    };

    return v;
}

static struct vec3
vec3_mul_scalar(struct vec3 v1, float s)
{
    struct vec3 v = {
        .x = v1.x * s,
        .y = v1.y * s,
        .z = v1.z * s,
    };

    return v;
}

struct ray {
    struct vec3 origin;
    struct vec3 direction;
};

static struct vec3
ray_at(struct ray r, float t)
{
    return vec3_add(r.origin, vec3_mul_scalar(r.direction, t));
}

struct camera {
    struct vec3 origin;
    struct vec3 horizontal;
    struct vec3 vertical;
    struct vec3 lower_left_corner;
};

int
main(int argc, char* argv[])
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    struct camera c = {
        .origin = { 0.0f, 0.0f, 0.0f },
        .horizontal = { 4.0f, 0.0f, 0.0f },
        .vertical = { 0.0f, 2.0f, 0.0f },
        .lower_left_corner = { -2.0f, -1.0f, -1.0f },
    };

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
