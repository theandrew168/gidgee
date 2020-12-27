#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glcorearb.h>
#include <GLFW/glfw3.h>

#include "opengl_loader.h"
#include "opengl_renderer.h"

int
opengl_renderer_init(struct opengl_renderer* renderer, GLFWwindow* window)
{
    assert(renderer != NULL);
    assert(window != NULL);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
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

    renderer->window = window;
    return 0;
}

int
opengl_renderer_term(struct opengl_renderer* renderer)
{
    assert(renderer != NULL);
    return 0;
}
