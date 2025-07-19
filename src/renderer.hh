#pragma once

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "vertex.hh"
#include "vertexarray.hh"
#include "vertexbuffer.hh"
#include "shader.hh"
#include "texture.hh"
#include "camera.hh"
#include "main.hh"

class Renderer {
    std::span<const Vertex> m_vertices;
    Shader m_shader { "shader.vert",  "shader.frag" };
    VertexArray m_vao;
    VertexBuffer m_vbo { m_vertices };

public:
    Renderer(std::span<const Vertex> vertices) : m_vertices(vertices)
    {
        m_shader.use();
        m_shader.set_uniform("tex", 0);

        GLuint pos = m_shader.get_attrib_loc("a_pos");
        GLuint uv  = m_shader.get_attrib_loc("a_uv");

        m_vao.add<float>(pos, 3);
        m_vao.add<float>(uv, 2);
    }

    void render(Texture& texture, State& state, glm::vec3 pos) {

        auto view = state.cam.get_view_matrix();

        float aspect_ratio = static_cast<float>(WIDTH) / HEIGHT;
        auto proj = glm::perspective(glm::radians(state.fov_deg), aspect_ratio, 0.1f, 100.0f);

        glm::mat4 model(1.0f);
        model = glm::translate(model, pos);

        auto u_mvp = proj * view * model;
        m_shader.set_uniform("u_mvp", u_mvp);

        texture.bind();
        m_shader.use();
        m_vao.bind();

        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size());
    }

};




