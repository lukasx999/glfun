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
    VertexBuffer(std::span<Vertex> vertices);
    ~VertexBuffer();
    VertexBuffer &bind();
    VertexBuffer &unbind();

};
