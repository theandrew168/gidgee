#ifndef OPENGL_SHADER_H_INCLUDED
#define OPENGL_SHADER_H_INCLUDED

unsigned int opengl_shader_compile_and_link(const char* vertex_source, const char* fragment_source);
void opengl_shader_set_int(unsigned int shader, const char* name, int value);
void opengl_shader_set_float(unsigned int shader, const char* name, float value);

#endif
