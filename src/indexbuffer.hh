#pragma once

#include <string>
#include <span>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>



class IndexBuffer {
    GLuint m_id;
    size_t m_count;

public:
    IndexBuffer(std::span<unsigned int> indices) : m_count(indices.size()) {
        glGenBuffers(1, &m_id);
        bind();
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            m_count * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW
        );
    }

    ~IndexBuffer() {
        glDeleteBuffers(1, &m_id);
    }

    IndexBuffer &bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        return *this;
    }

    IndexBuffer &unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return *this;
    }

};
