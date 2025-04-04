#pragma once

#include <span>

#include "glad/gl.h"



struct IndexBuffer {

    GLuint m_id;
    size_t m_count;

    IndexBuffer(std::span<unsigned int> indices);
    ~IndexBuffer();
    IndexBuffer &bind();
    IndexBuffer &unbind();

};
