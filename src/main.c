#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GLFW/glfw3.h>

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

static struct vec3
vec3_neg(struct vec3 v1)
{
    struct vec3 v = {
        .x = -v1.x,
        .y = -v1.y,
        .z = -v1.z,
    };

    return v;
}

static float
vec3_dot(struct vec3 v1, struct vec3 v2)
{
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

static float
vec3_magnitude_squared(struct vec3 v1)
{
    return vec3_dot(v1, v1);
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

struct ray {
    struct vec3 origin;
    struct vec3 direction;
};

static struct vec3
ray_at(struct ray r, float t)
{
    return vec3_add(r.origin, vec3_mul_scalar(r.direction, t));
}

struct sphere {
    struct vec3 center;
    float radius;
};

struct intersection {
    struct vec3 point;
    struct vec3 normal;
    float t;
    bool front_face;
};

static void
intersect_set_face_normal(struct intersection* i, struct ray r, struct vec3 outward_normal)
{
    assert(i != NULL);

    i->front_face = vec3_dot(r.direction, outward_normal) < 0.0f;
    i->normal = i->front_face ? outward_normal : vec3_neg(outward_normal);
}

static bool
intersect_ray_sphere(struct intersection* i, struct ray r, struct sphere s, float t_min, float t_max)
{
    assert(i != NULL);

    struct vec3 oc = vec3_sub(r.origin, s.center);
    float a = vec3_magnitude_squared(r.direction);
    float half_b = vec3_dot(oc, r.direction);
    float c = vec3_magnitude_squared(oc) - (s.radius * s.radius);
    float discriminant = (half_b * half_b) - (a * c);

    // early exit if no roots (aka no intersection points)
    if (discriminant < 0.0f) return false;
    float sqrtd = sqrtf(discriminant);

    // find nearest root that lies within [t_min, t_max]
    float root = (-half_b - sqrtd) / a;
    if (root < t_min || root > t_max) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || root > t_max) {
            return false;
        }
    }

    i->t = root;
    i->point = ray_at(r, i->t);
    struct vec3 outward_normal = vec3_div_scalar(vec3_sub(i->point, s.center), s.radius);
    intersect_set_face_normal(i, r, outward_normal);
    return true;
}

static struct vec3
ray_color(struct ray r, struct sphere* spheres, long count)
{
    struct intersection in = { 0 };
    bool hit_anything = false;
    float closest = FLT_MAX;

    for (long i = 0; i < count; i++) {
        if (intersect_ray_sphere(&in, r, spheres[i], 0.001f, closest)) {
            hit_anything = true;
            closest = in.t;
        }
    }

    if (hit_anything) {
        return vec3_mul_scalar(vec3_add(in.normal, (struct vec3){ 1.0f, 1.0f, 1.0f }), 0.5f);
    }

    struct vec3 unit_direction = vec3_normalize(r.direction);
    float t = 0.5f * (unit_direction.y + 1.0f);
    struct vec3 black = { 1.0f, 1.0f, 1.0f };
    struct vec3 color = { 0.5f, 0.7f, 1.0f };
    return vec3_add(vec3_mul_scalar(black, 1.0f - t), vec3_mul_scalar(color, t));
}

static void
print_usage(const char* arg0)
{
    printf("usage: %s [options]\n", arg0);
    printf("\n");
    printf("Options:\n");
    printf("  -h --help        print this help\n");
    printf("  -f --fullscreen  fullscreen window\n");
    printf("  -v --vsync       enable vsync\n");
}

int
main(int argc, char* argv[])
{
    bool fullscreen = false;
    bool vsync = false;

    // process CLI args and update corresponding flags
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0) {
            fullscreen = true;
        }
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--vsync") == 0) {
            vsync = true;
        }
    }

	if (!glfwInit()) {
		const char* error = NULL;
		glfwGetError(&error);
		fprintf(stderr, "failed to init GLFW3: %s\n", error);
		return EXIT_FAILURE;
	}

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    // ask for an OpenGL 3.3 Core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    // set flags for monitor details
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = NULL;
    if (fullscreen) {
        window = glfwCreateWindow(mode->width, mode->height, "Gidgee Physics", monitor, NULL);
    } else {
        window = glfwCreateWindow(1280, 720, "Gidgee Physics", NULL, NULL);
    }

    if (window == NULL) {
        const char* error = NULL;
        glfwGetError(&error);
        fprintf(stderr, "failed to create GLFW3 window: %s\n", error);

        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vsync ? 1 : 0);

    double last_second = glfwGetTime();
    double last_frame = last_second;
    long frame_count = 0;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        double now = glfwGetTime();
        double delta = now - last_frame;
        last_frame = now;

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        frame_count++;
        if (glfwGetTime() - last_second >= 1.0) {
            printf("FPS: %ld  (%lf ms/frame)\n", frame_count, 1000.0/frame_count);
            frame_count = 0;
            last_second += 1.0;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
