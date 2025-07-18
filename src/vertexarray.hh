#pragma once

#include "glad/gl.h"
#include "vertex.hh"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>



class VertexArray {
    GLuint m_id;
    size_t m_offset = 0;

public:
    VertexArray() {
        glGenVertexArrays(1, &m_id);
    }

    ~VertexArray() {
        glDeleteVertexArrays(1, &m_id);
    }

    VertexArray(VertexArray const&) = delete;
    VertexArray& operator=(VertexArray const&) = delete;

    VertexArray& bind() {
        glBindVertexArray(m_id);
        return *this;
    }

    VertexArray& unbind() {
        glBindVertexArray(0);
        return *this;
    }

    template <typename T>
    VertexArray &add(GLuint location, GLint components) = delete;

private:
    void add_attr(GLuint location, GLint components, GLenum type, size_t elem_size) {
        bind();

        glVertexAttribPointer(location, components, type, false, sizeof(Vertex),
                              reinterpret_cast<void*>(m_offset));

        glEnableVertexAttribArray(location);
        m_offset += elem_size * components;
    }

};

template <>
inline VertexArray& VertexArray::add<float>(GLuint location, GLint components) {
    add_attr(location, components, GL_FLOAT, sizeof(float));
    return *this;
}
