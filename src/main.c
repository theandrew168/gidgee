#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glcorearb.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "opengl_buffer.h"
#include "opengl_renderer.h"
#include "opengl_shader.h"
#include "opengl_texture.h"

static void
print_usage(const char* arg0)
{
    printf("usage: %s [options]\n", arg0);
    printf("\n");
    printf("Options:\n");
    printf("  -h  --help        print this help\n");
    printf("  -f  --fullscreen  fullscreen window\n");
    printf("  -r  --resizable   resizable window\n");
    printf("  -vk --vulkan      use vulkan renderer\n");
}

int
main(int argc, char* argv[])
{
    bool fullscreen = false;
    bool resizable = false;
    bool use_vk = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--fullscreen") == 0) {
            fullscreen = true;
        }
        if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--resizable") == 0) {
            resizable = true;
        }
        if (strcmp(argv[i], "-vk") == 0 || strcmp(argv[i], "--vulkan") == 0) {
            use_vk = true;
        }
    }

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW3!\n");
        return EXIT_FAILURE;
    }

    if (use_vk) {
        fprintf(stderr, "Vulkan renderer isn't implemented yet :(\n");
        return EXIT_FAILURE;
    }

    if (use_vk && !glfwVulkanSupported()) {
        fprintf(stderr, "Vulkan requested but not available!\n");
        fprintf(stderr, "Please install a recent version of the Vulkan SDK:\n");
        fprintf(stderr, "https://vulkan.lunarg.com/sdk/home\n");
        return EXIT_FAILURE;
    }

    if (use_vk) {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    } else {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    glfwWindowHint(GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);

    GLFWwindow* window = NULL;
    if (fullscreen) {
        window = glfwCreateWindow(mode->width, mode->height, "Gidgee Physics", monitor, NULL);
    } else {
        window = glfwCreateWindow(1280, 720, "Gidgee Physics", NULL, NULL);
    }

    if (window == NULL) {
        fprintf(stderr, "failed to create GLFW3 window!\n");
        return EXIT_FAILURE;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    struct opengl_renderer renderer = { 0 };
    if (opengl_renderer_init(&renderer, window) != 0) {
        fprintf(stderr, "failed to init OpenGL renderer\n");
        return EXIT_FAILURE;
    }

    double last_second = 0.0;
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

        opengl_renderer_clear(&renderer);

        frame_count++;
        if (glfwGetTime() - last_second >= 1.0) {
            printf("FPS: %ld  (%lf ms/frame)\n", frame_count, 1000.0/frame_count);
            frame_count = 0;
            last_second += 1.0;
        }

        opengl_renderer_present(&renderer);
        glfwPollEvents();
    }

    opengl_renderer_term(&renderer);
    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}
