#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glcorearb.h>

#include "opengl_loader.h"


enum {
    INFO_LOG_SIZE = 1024,
};

static bool
shader_compile_source(unsigned int shader, const char* source)
{
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        char info_log[INFO_LOG_SIZE] = { 0 };
        glGetShaderInfoLog(shader, INFO_LOG_SIZE, NULL, info_log);

        fprintf(stderr, "failed to compile shader:\n%s\n", info_log);
        return false;
    }

    return true;
}

static bool
shader_link_program(unsigned int program, unsigned int vertex_shader, unsigned int fragment_shader)
{
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char info_log[INFO_LOG_SIZE] = { 0 };
        glGetProgramInfoLog(program, INFO_LOG_SIZE, NULL, info_log);

        glDetachShader(program, vertex_shader);
        glDetachShader(program, fragment_shader);

        fprintf(stderr, "failed to link program:\n%s\n", info_log);
        return false;
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);
    return true;
}

unsigned int
opengl_shader_compile_and_link(const char* vertex_source, const char* fragment_source)
{
    assert(vertex_source != NULL);
    assert(fragment_source != NULL);

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    shader_compile_source(vs, vertex_source);
    shader_compile_source(fs, fragment_source);

    unsigned int prog = glCreateProgram();
    shader_link_program(prog, vs, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return prog;
}
