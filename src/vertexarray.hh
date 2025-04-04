#pragma once

#include <iostream>

#include "glad/gl.h"



struct VertexArray {

    GLuint m_id;
    size_t m_offset;

    VertexArray();
    ~VertexArray();
    VertexArray &push_attr(GLuint loc, GLint size, GLenum type);
    VertexArray &bind();
    VertexArray &unbind();

private:
    inline size_t sizeof_gltype(GLenum type);

};
