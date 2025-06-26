#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>



class Camera {
    glm::vec3 m_position;
    glm::vec3 m_direction { 0.0f };
    float m_yaw = -90.0f;
    float m_pitch = 0.0f;
    static constexpr glm::vec3 m_up { 0.0f, 1.0f, 0.0f };
    static constexpr float m_sensitivity = 0.05f;
    static constexpr float m_speed = 15.0f;

public:
    Camera(glm::vec3 position)
    : m_position(position)
    {
        update_rotation();
    }

    [[nodiscard]] glm::mat4 get_view_matrix() const {
        return glm::lookAt(m_position, m_position + m_direction, m_up);
    }

    void rotate(glm::vec2 mouse_delta) {
        m_yaw   += mouse_delta.x * m_sensitivity;
        m_pitch -= mouse_delta.y * m_sensitivity;
        update_rotation();
    }

    void move_forward(float dt) {
        m_position += m_direction * m_speed * dt;
    }

    void move_backward(float dt) {
        m_position -= m_direction * m_speed * dt;
    }

    void move_right(float dt) {
        m_position += glm::normalize(glm::cross(m_direction, m_up)) * m_speed * dt;
    }

    void move_left(float dt) {
        m_position -= glm::normalize(glm::cross(m_direction, m_up)) * m_speed * dt;
    }

private:
    void update_rotation() {
        m_direction.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_direction.y = sin(glm::radians(m_pitch));
        m_direction.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_direction = glm::normalize(m_direction);
    }

};
