#ifndef OPENGL_TEXTURE_H_INCLUDED
#define OPENGL_TEXTURE_H_INCLUDED

// common enum, move to texture.h
enum texture_format {
    TEXTURE_FORMAT_UNDEFINED = 0,
    TEXTURE_FORMAT_RGB,
    TEXTURE_FORMAT_RGBA,
};

unsigned int opengl_texture_create(int format, long width, long height, const unsigned char* pixels);

#endif
