#pragma once

#include <string>
#include <span>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"



enum class Color {
    BLACK,
    WHITE,
    RED,
    BLUE,
    GREEN,
    MAGENTA,
    YELLOW,
    CYAN,
};

[[nodiscard]] constexpr glm::vec3 color_to_vec3(Color color);

struct Vertex {

    glm::vec3 m_pos;
    glm::vec2 m_uv;
    glm::vec3 m_color;

    Vertex(glm::vec3 pos, glm::vec2 uv, Color color);
    Vertex(glm::vec3 pos, glm::vec2 uv);
    Vertex &rotate(float angle, glm::vec3 normal);

};
