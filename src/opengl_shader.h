#ifndef OPENGL_SHADER_H_INCLUDED
#define OPENGL_SHADER_H_INCLUDED

#include <linmath/linmath.h>

unsigned int opengl_shader_compile_and_link(const char* vertex_source, const char* fragment_source);
void opengl_shader_set_int(unsigned int shader, const char* name, int value);
void opengl_shader_set_float(unsigned int shader, const char* name, float value);
void opengl_shader_set_mat4(unsigned int shader, const char* name, mat4x4 value);

#endif
