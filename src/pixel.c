#include "pixel.h"

long
pixel_size(int format)
{
    switch(format) {
    case PIXEL_FORMAT_RGB:
        return 3 * sizeof(float);
    case PIXEL_FORMAT_RGBA:
        return 4 * sizeof(float);
    default:
        return 0;
    }
}
