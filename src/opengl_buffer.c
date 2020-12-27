#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glcorearb.h>

#include "opengl_buffer.h"
#include "opengl_loader.h"
#include "vertex.h"

#define BUFFER_OFFSET(offset) (void*)((size_t)offset)

unsigned int
opengl_buffer_create(int vertex_format, const float* vertices, long count)
{
    long size = count * vertex_size(vertex_format);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

    return vbo;
}

unsigned int
opengl_buffer_config(int vertex_format, unsigned int buffer)
{
    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    long stride = vertex_size(vertex_format);
    long offset = 0;

    if (vertex_format & VERTEX_FORMAT_POSITION) {
        glVertexAttribPointer(VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
        glEnableVertexAttribArray(VERTEX_ATTRIB_POSITION);
        offset += 3 * sizeof(float);
    }
    if (vertex_format & VERTEX_FORMAT_TEXCOORD) {
        glVertexAttribPointer(VERTEX_ATTRIB_TEXCOORD, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
        glEnableVertexAttribArray(VERTEX_ATTRIB_TEXCOORD);
        offset += 2 * sizeof(float);
    }
    if (vertex_format & VERTEX_FORMAT_NORMAL) {
        glVertexAttribPointer(VERTEX_ATTRIB_NORMAL, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
        glEnableVertexAttribArray(VERTEX_ATTRIB_NORMAL);
        offset += 3 * sizeof(float);
    }
    if (vertex_format & VERTEX_FORMAT_COLOR) {
        glVertexAttribPointer(VERTEX_ATTRIB_COLOR, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(offset));
        glEnableVertexAttribArray(VERTEX_ATTRIB_COLOR);
        offset += 3 * sizeof(float);
    }

    // unbind VAO to capture state
    glBindVertexArray(0);
    return vao;
}
