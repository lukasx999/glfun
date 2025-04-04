#pragma once

#include <string>

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

inline glm::vec3 color_to_vec3(Color color) {
    switch (color) {
        case Color::BLACK:   return glm::vec3(0.0f);
        case Color::WHITE:   return glm::vec3(1.0f);
        case Color::RED:     return glm::vec3(1.0f, 0.0f, 0.0f);
        case Color::BLUE:    return glm::vec3(0.0f, 0.0f, 1.0f);
        case Color::GREEN:   return glm::vec3(0.0f, 1.0f, 0.0f);
        case Color::MAGENTA: return glm::vec3(1.0f, 0.0f, 1.0f);
        case Color::YELLOW:  return glm::vec3(1.0f, 1.0f, 0.0f);
        case Color::CYAN:    return glm::vec3(0.0f, 1.0f, 1.0f);
        default:             assert(!"unknown color");
    }
}

struct Vertex {
    glm::vec3 m_pos;
    glm::vec2 m_tex_coords;
    glm::vec3 m_color;

    Vertex(glm::vec3 pos, glm::vec2 tex_coords, Color color)
        : m_pos(pos)
        , m_tex_coords(tex_coords)
        , m_color(color_to_vec3(color))
    {}

    Vertex &rotate(float angle, glm::vec3 normal) {
        m_pos = glm::rotate(m_pos, angle, normal);
        return *this;
    }

};
