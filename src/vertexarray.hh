#pragma once

#include <string>
#include <span>

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

    VertexArray &push_attr(GLuint loc, GLint size, GLenum type);
    VertexArray &bind();
    VertexArray &unbind();

private:
    [[nodiscard]] constexpr size_t sizeof_gltype(GLenum type) const;

};
