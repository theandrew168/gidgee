#ifndef PIXEL_H_INCLUDED
#define PIXEL_H_INCLUDED

enum pixel_format {
    PIXEL_FORMAT_UNDEFINED = 0,
    PIXEL_FORMAT_RGB,
    PIXEL_FORMAT_RGBA,
};

long pixel_size(int format);

#endif
