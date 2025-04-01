#include <iostream>
#include <print>
#include <fstream>
#include <vector>
#include <array>
#include <cassert>
#include <span>


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

#include "shader.hh"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"





constexpr int width  = 1600;
constexpr int height = 900;





GLFWwindow *setup_window() {

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Cube", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSwapInterval(1);
    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(
        window,
        [](GLFWwindow* _win, int w, int h) {
            (void) _win;
            glViewport(0, 0, w, h);
        }
    );

    return window;
}

enum class Color {
    RED,
    BLUE,
    GREEN,
    MAGENTA,
    YELLOW,
    CYAN,
};

glm::vec3 color_to_vec3(Color color) {
    switch (color) {
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
public:
    glm::vec3 m_pos;
    glm::vec2 m_tex_coords;
    glm::vec3 m_color;

    Vertex(glm::vec3 pos, glm::vec2 tex_coords, Color color)
        : m_pos(pos)
        , m_tex_coords(tex_coords)
        , m_color(color_to_vec3(color))
    {}

    void rotate(float angle, glm::vec3 normal) {
        m_pos = glm::rotate(m_pos, angle, normal);
    }

};

bool is_key_pressed(GLFWwindow *window, int key) {

    static bool old = false;
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool ret = !old && pressed;
    old = pressed;
    return ret;
}

void process_inputs(GLFWwindow *window, Vertex *vertices, size_t v_size) {
    static bool mode = false;

    if (is_key_pressed(window, GLFW_KEY_K))
        mode = !mode;

    if (mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        for (size_t i=0; i < v_size; ++i) {
            vertices[i].rotate(0.01f, { 1.0f, 1.0f, 0.0f });
        }
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}


class Texture {

    GLuint m_texture;
    GLenum m_unit;

public:
    Texture(
        GLenum unit,
        const char *filename,
        bool flip_vert,
        int format,
        std::span<std::pair<GLenum, GLint>> params
    )
    : m_texture(load_texture(filename, flip_vert, format, params))
    , m_unit(unit)
    {}

    void set_uniform(ShaderProgram &shader, const char *name) {
        // TODO: get texture unit GLenum as integer
        shader
            .use()
            .set_uniform_int(name, 0)
            .set_uniform_int("tex_face", 1);
    }

    void bind() {
        glActiveTexture(m_unit);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

private:
    GLuint load_texture(const char *filename, bool flip_vert, int format, std::span<std::pair<GLenum, GLint>> params) {

        stbi_set_flip_vertically_on_load(flip_vert);

        int width, height, nrChannels;
        uint8_t *data = stbi_load(filename, &width, &height, &nrChannels, 0); 

        if (data == nullptr)
            std::println("Failed to load image {}", filename);

        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        for (auto [name, value] : params)
            glTexParameteri(GL_TEXTURE_2D, name, value);

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
        return texture;
    }

};



int main() {

    std::array vertices {
        Vertex({  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, Color::RED),   // top right
        Vertex({  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, Color::BLUE),  // bottom right
        Vertex({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, Color::GREEN), // bottom left
        Vertex({ -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, Color::CYAN),  // top left
    };

    std::array indices {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };


    GLFWwindow *window = setup_window();

    ShaderProgram shader("vert.glsl", "frag.glsl");

    std::array<std::pair<GLenum, GLint>, 4> tex_params {
        std::pair { GL_TEXTURE_WRAP_S,     GL_REPEAT },
        std::pair { GL_TEXTURE_WRAP_T,     GL_REPEAT },
        std::pair { GL_TEXTURE_MIN_FILTER, GL_LINEAR },
        std::pair { GL_TEXTURE_MAG_FILTER, GL_LINEAR }
    };

    Texture tex_container(GL_TEXTURE0, "container.jpg",   false, GL_RGB,  std::span(tex_params));
    Texture tex_face     (GL_TEXTURE1, "awesomeface.png", true,  GL_RGBA, std::span(tex_params));

    shader.use();
    shader.set_uniform_int("tex_container", 0);
    shader.set_uniform_int("tex_face", 1);

    GLuint vao, vbo, ebo;
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);




    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glBindVertexArray(vao);
        shader.use();

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW); 


        GLuint pos_loc = shader.get_attrib_loc("a_pos");
        glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(pos_loc);

        GLuint tex_loc = shader.get_attrib_loc("a_tex_coords");
        glVertexAttribPointer(tex_loc, 2, GL_FLOAT, false, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, m_tex_coords)));
        glEnableVertexAttribArray(tex_loc);

        GLuint col_loc = shader.get_attrib_loc("a_col");
        glVertexAttribPointer(col_loc, 3, GL_FLOAT, false, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, m_color)));
        glEnableVertexAttribArray(col_loc);


        tex_container.bind();
        tex_face.bind();

        shader.use();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        process_inputs(window, vertices.data(), vertices.size());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
