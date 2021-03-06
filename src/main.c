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
#include "opengl_loader.h"
#include "opengl_shader.h"
#include "opengl_texture.h"

// resources
#include "models/square.h"
#include "shaders/dev_frag.h"
#include "shaders/dev_vert.h"
#include "textures/box_diffuse.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct app {
    long vertex_count;
    unsigned vbo;
    unsigned vao;
    unsigned shader;
    unsigned texture;
};

void
app_init(struct app* app)
{
    unsigned int vbo = opengl_buffer_create(
        MODELS_SQUARE_VERTEX_FORMAT,
        MODELS_SQUARE_VERTICES,
        MODELS_SQUARE_VERTEX_COUNT);
    unsigned int vao = opengl_buffer_config(
        MODELS_SQUARE_VERTEX_FORMAT,
        vbo);

    unsigned int shader = opengl_shader_compile_and_link(
        SHADERS_DEV_VERT_SOURCE,
        SHADERS_DEV_FRAG_SOURCE);
    opengl_shader_set_int(shader, "u_texture", 0);

    mat4x4 mvp = {{ 0 }};
    mat4x4_identity(mvp);
    mat4x4_rotate(mvp, mvp, 0.0f, 0.0f, 1.0f, 90.0f * (M_PI / 180.0));
    mat4x4_scale_aniso(mvp, mvp, 0.5f, 0.5f, 0.5f);
    opengl_shader_set_mat4(shader, "u_mvp", mvp);

    unsigned int texture = opengl_texture_create(
        TEXTURES_BOX_DIFFUSE_PIXEL_FORMAT,
        TEXTURES_BOX_DIFFUSE_WIDTH,
        TEXTURES_BOX_DIFFUSE_HEIGHT,
        TEXTURES_BOX_DIFFUSE_PIXELS);

    app->vertex_count = MODELS_SQUARE_VERTEX_COUNT;
    app->vbo = vbo;
    app->vao = vao;
    app->shader = shader;
    app->texture = texture;
}

void
app_update(struct app* app, double delta)
{
    // unused vars... for now
    (void)app;
    (void)delta;
}

void
app_render(struct app* app)
{
    glUseProgram(app->shader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, app->texture);
    glBindVertexArray(app->vao);

    glDrawArrays(GL_TRIANGLES, 0, app->vertex_count);
}

static void
print_usage(const char* arg0)
{
    printf("usage: %s [options]\n", arg0);
    printf("\n");
    printf("Options:\n");
    printf("  -h  --help        print this help\n");
    printf("  -f  --fullscreen  fullscreen window\n");
    printf("  -r  --resizable   resizable window\n");
    printf("  -v  --vsync       disable vsync\n");
    printf("  -vk --vulkan      use vulkan renderer\n");
}

int
main(int argc, char* argv[])
{
    bool fullscreen = false;
    bool resizable = false;
    bool vsync = true;
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
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--vsync") == 0) {
            vsync = false;
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
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vsync ? 1 : 0);
    opengl_loader_load_functions();

    printf("OpenGL Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version:  %s\n", glGetString(GL_VERSION));
    printf("GLSL Version:    %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    struct app app = { 0 };
    app_init(&app);

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

        app_update(&app, delta);

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        app_render(&app);

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
