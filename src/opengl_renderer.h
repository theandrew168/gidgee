#ifndef OPENGL_RENDERER_H_INCLUDED
#define OPENGL_RENDERER_H_INCLUDED

#include <GL/glcorearb.h>
#include <GLFW/glfw3.h>

struct opengl_renderer {
    GLFWwindow* window;
};

int opengl_renderer_init(struct opengl_renderer* renderer, GLFWwindow* window);
int opengl_renderer_term(struct opengl_renderer* renderer);

#endif
