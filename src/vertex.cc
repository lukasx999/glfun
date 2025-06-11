#include "vertex.hh"




[[nodiscard]] constexpr glm::vec3 color_to_vec3(Color color) {
    switch (color) {
        case Color::BLACK:   return glm::vec3(0.0f);
        case Color::WHITE:   return glm::vec3(1.0f);
        case Color::RED:     return glm::vec3(1.0f, 0.0f, 0.0f);
        case Color::BLUE:    return glm::vec3(0.0f, 0.0f, 1.0f);
        case Color::GREEN:   return glm::vec3(0.0f, 1.0f, 0.0f);
        case Color::MAGENTA: return glm::vec3(1.0f, 0.0f, 1.0f);
        case Color::YELLOW:  return glm::vec3(1.0f, 1.0f, 0.0f);
        case Color::CYAN:    return glm::vec3(0.0f, 1.0f, 1.0f);
    }
}

Vertex::Vertex(glm::vec3 pos, glm::vec2 uv, Color color)
    : m_pos(pos)
    , m_uv(uv)
    , m_color(color_to_vec3(color))
{ }

Vertex::Vertex(glm::vec3 pos, glm::vec2 uv)
    : m_pos(pos)
    , m_uv(uv)
    , m_color(color_to_vec3(Color::BLACK))
{ }

Vertex::Vertex(glm::vec3 pos)
    : m_pos(pos)
    , m_uv(glm::vec3(0))
    , m_color(color_to_vec3(Color::BLACK))
{ }

Vertex &Vertex::rotate(float angle, glm::vec3 normal) {
    m_pos = glm::rotate(m_pos, angle, normal);
    return *this;
}
