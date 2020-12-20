#include <assert.h>
#include <math.h>
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

static struct vec3
vec3_div_scalar(struct vec3 v1, float s)
{
    struct vec3 v = {
        .x = v1.x / s,
        .y = v1.y / s,
        .z = v1.z / s,
    };

    return v;
}

static float
vec3_length_squared(struct vec3 v1)
{
    return (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z);
}

static float
vec3_length(struct vec3 v1)
{
    return sqrtf(vec3_length_squared(v1));
}

static struct vec3
vec3_unit(struct vec3 v1)
{
    return vec3_div_scalar(v1, vec3_length(v1));
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

static struct vec3
ray_color(struct ray r)
{
    struct vec3 unit_direction = vec3_unit(r.direction);
    float t = 0.5f * (unit_direction.y + 1.0f);
    struct vec3 black = { 1.0f, 1.0f, 1.0f };
    struct vec3 color = { 0.5f, 0.7f, 1.0f };
    return vec3_add(vec3_mul_scalar(black, 1.0f - t), vec3_mul_scalar(color, t));
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

    float aspect_ratio = 16.0f / 9.0f;
    long image_width = 1280;
    long image_height = 720;

    float viewport_width = 16.0f;
    float viewport_height = 9.0f;
    float focal_length = 1.0f;

    struct vec3 origin = { 0.0f, 0.0f, 0.0f };
//    struct vec3 horizontal = { viewport_width, 0.0f, 0.0f };
//    struct vec3 vertical = { 0.0f, viewport_height, 0.0f };
    struct vec3 lower_left_corner = { -8.0f, -4.5f, -1.0f };

    ppm_write_header(stdout, image_width, image_height);
    for (long y = image_height - 1; y >= 0; y--) {
        fprintf(stderr, "lines remaining: %ld\n", y);
        for (long x = 0; x < image_width; x++) {
            float u = (float)x / (image_width - 1);
            float v = (float)y / (image_height - 1);
            struct ray r = {
                .origin = origin,
                .direction = { -8.0f + u * viewport_width, -4.5f + v * viewport_height, -1.0f },
            };

            struct vec3 pixel_color = ray_color(r);
            ppm_write_pixel(stdout, pixel_color.x, pixel_color.y, pixel_color.z);
        }
    }

    return EXIT_SUCCESS;
}
