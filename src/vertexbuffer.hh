#pragma once

#include <string>
#include <span>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "vertex.hh"



class VertexBuffer {
    GLuint m_id;

public:
    VertexBuffer(std::span<const Vertex> vertices) {
        glGenBuffers(1, &m_id);
        bind();
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                     vertices.data(), GL_STATIC_DRAW);
    }

    ~VertexBuffer() {
        glDeleteBuffers(1, &m_id);
    }

    VertexBuffer(VertexBuffer const&) = delete;
    VertexBuffer& operator=(VertexBuffer const&) = delete;

    VertexBuffer &bind() {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        return *this;
    }

    VertexBuffer &unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return *this;
    }

};
