#include "vertex.h"

long
vertex_size(int format)
{
    long size = 0;
    if (format & VERTEX_FORMAT_POSITION) size += 3;
    if (format & VERTEX_FORMAT_TEXCOORD) size += 2;
    if (format & VERTEX_FORMAT_NORMAL) size += 3;
    if (format & VERTEX_FORMAT_COLOR) size += 3;
    return size * sizeof(float);
}
