#pragma once

#include <span>

#include "glad/gl.h"

#include "vertex.hh"



struct VertexBuffer {

    GLuint m_id;

    VertexBuffer(std::span<Vertex> vertices);
    ~VertexBuffer();
    VertexBuffer &bind();
    VertexBuffer &unbind();

};
