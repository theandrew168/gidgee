#ifndef OPENGL_BUFFER_H_INCLUDED
#define OPENGL_BUFFER_H_INCLUDED

// common enum, move to buffer.h
enum buffer_format {
    BUFFER_FORMAT_UNDEFINED = 0,
    BUFFER_FORMAT_V3F,
    BUFFER_FORMAT_T2F_V3F,
    BUFFER_FORMAT_N3F_V3F,
    BUFFER_FORMAT_T2F_N3F_V3F,
};

unsigned int opengl_buffer_create(int format, long count, const float* vertices);
unsigned int opengl_buffer_config(int format, int buffer);

#endif
