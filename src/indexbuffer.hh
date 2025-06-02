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
    IndexBuffer(std::span<unsigned int> indices);
    ~IndexBuffer();

    IndexBuffer &bind();
    IndexBuffer &unbind();

};
