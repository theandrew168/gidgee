#ifndef OPENGL_BUFFER_H_INCLUDED
#define OPENGL_BUFFER_H_INCLUDED

unsigned int opengl_buffer_create(int vertex_format, const float* vertices, long count);
unsigned int opengl_buffer_config(int vertex_format, unsigned int buffer);

#endif
