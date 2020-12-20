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
vec3_sub(struct vec3 v1, struct vec3 v2)
{
    struct vec3 v = {
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
        .z = v1.z - v2.z,
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
vec3_magnitude_squared(struct vec3 v1)
{
    return (v1.x * v1.x) + (v1.y * v1.y) + (v1.z * v1.z);
}

static float
vec3_magnitude(struct vec3 v1)
{
    return sqrtf(vec3_magnitude_squared(v1));
}

static struct vec3
vec3_normalize(struct vec3 v1)
{
    return vec3_div_scalar(v1, vec3_magnitude(v1));
}

static float
vec3_dot(struct vec3 v1, struct vec3 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
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

static float
ray_hit_sphere(struct ray r, struct vec3 center, float radius)
{
    struct vec3 oc = vec3_sub(r.origin, center);
    float a = vec3_magnitude_squared(r.direction);
    float half_b = vec3_dot(oc, r.direction);
    float c = vec3_magnitude_squared(oc) - (radius * radius);
    float discriminant = (half_b * half_b) - (a * c);
    if (discriminant < 0.0f) {
        return -1.0f;
    } else {
        return (-half_b - sqrtf(discriminant)) / a;
    }
}

static struct vec3
ray_color(struct ray r)
{
    float t = ray_hit_sphere(r, (struct vec3){ 0.0f, 0.0f, -1.0f }, 0.5f);
    if (t > 0.0f) {
        struct vec3 n = vec3_normalize(vec3_sub(ray_at(r, t), (struct vec3){ 0.0f, 0.0f, -1.0f }));
        return vec3_mul_scalar((struct vec3){ n.x + 1.0f, n.y + 1.0f, n.z + 1.0f }, 0.5f);
    }

    struct vec3 unit_direction = vec3_normalize(r.direction);
    t = 0.5f * (unit_direction.y + 1.0f);
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

    // output image dimensions
    long image_width = 1280;
    long image_height = 720;

    // world-space viewport bounds
    float aspect_ratio = 16.0f / 9.0f;
    float viewport_height = 2.0f;
    float viewport_width = viewport_height * aspect_ratio;
    float focal_length = 1.0f;

    ppm_write_header(stdout, image_width, image_height);
    for (long y = image_height - 1; y >= 0; y--) {
        fprintf(stderr, "lines remaining: %ld\n", y);
        for (long x = 0; x < image_width; x++) {
            float u = (float)x / (image_width - 1);
            float v = (float)y / (image_height - 1);
            struct ray r = {
                .origin = { 0.0f, 0.0f, 0.0f },
                .direction = {
                    .x = -(viewport_width / 2.0f) + (u * viewport_width),
                    .y = -(viewport_height / 2.0f) + (v * viewport_height),
                    .z = -focal_length,
                },
            };

            struct vec3 pixel_color = ray_color(r);
            ppm_write_pixel(stdout, pixel_color.x, pixel_color.y, pixel_color.z);
        }
    }

    return EXIT_SUCCESS;
}
