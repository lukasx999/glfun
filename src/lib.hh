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

glm::vec3 color_to_vec3(Color color);

struct Vertex {

    glm::vec3 m_pos;
    glm::vec2 m_tex_coords;
    glm::vec3 m_color;

    Vertex(glm::vec3 pos, glm::vec2 tex_coords, Color color);
    Vertex(glm::vec3 pos, glm::vec2 tex_coords);
    Vertex &rotate(float angle, glm::vec3 normal);

};




struct VertexBuffer {

    GLuint m_id;

    VertexBuffer(std::span<Vertex> vertices);
    ~VertexBuffer();
    VertexBuffer &bind();
    VertexBuffer &unbind();

};



struct VertexArray {

    GLuint m_id;
    size_t m_offset;

    VertexArray();
    ~VertexArray();
    VertexArray &push_attr(GLuint loc, GLint size, GLenum type);
    VertexArray &bind();
    VertexArray &unbind();

private:
    inline size_t sizeof_gltype(GLenum type);

};



struct IndexBuffer {

    GLuint m_id;
    size_t m_count;

    IndexBuffer(std::span<unsigned int> indices);
    ~IndexBuffer();
    IndexBuffer &bind();
    IndexBuffer &unbind();

};



struct Shader {

    GLuint m_id;

    Shader(const char *vert, const char *frag);
    ~Shader();
    Shader &use();
    GLuint get_attrib_loc(const char *name) const;
    // make sure to use() before setting uniforms
    Shader &set_uniform_int(const char *name, int value);
    Shader &set_uniform_float(const char *name, float value);
    Shader &set_uniform_3f(const char *name, glm::vec3 value);
    Shader &set_uniform_mat4(const char *name, glm::mat4 value);

private:
    std::string read_entire_file(const char *filename);
    GLuint setup_shader(GLenum type, const char *filename);
};



class Texture {

    GLuint m_texture;
    GLenum m_unit;

public:
    // resize params are unused if one is 0
    Texture(GLenum unit, const char *filename, bool flip_vert, int format, int resize_width, int resize_height);
    Texture &bind();

private:
    GLuint load_texture(const char *filename, bool flip_vert, int format, int resize_width, int resize_height);

};
