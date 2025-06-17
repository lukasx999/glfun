#pragma once

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>



class VertexArray {
    GLuint m_id;
    size_t m_offset;

public:
    VertexArray();
    ~VertexArray();
    VertexArray &bind();
    VertexArray &unbind();

    template <typename T>
    VertexArray &add([[maybe_unused]] GLuint location, [[maybe_unused]] GLint components) {
        static_assert(false);
        return *this;
    }

private:
    void add_attr(GLuint location, GLint components, GLenum type, size_t elem_size);

};

template<>
VertexArray &VertexArray::add<float>(GLuint location, GLint components);
