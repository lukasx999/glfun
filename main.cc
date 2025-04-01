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

int main() {

    std::array vertices {
        Vertex({  0.5f,  0.5f, 0.0f }, {}, Color::RED), // top right
        Vertex({  0.5f, -0.5f, 0.0f }, {}, Color::RED), // bottom right
        Vertex({ -0.5f, -0.5f, 0.0f }, {}, Color::RED), // bottom left
        Vertex({ -0.5f,  0.5f, 0.0f }, {}, Color::RED), // top left
    };

    std::array indices {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };


    GLFWwindow *window = setup_window();



    const char *image_filename = "container.jpg";
    int width, height, nrChannels;
    uint8_t *data = stbi_load(image_filename, &width, &height, &nrChannels, 0); 
    if (data == nullptr) {
        std::println("Failed to load image {}", image_filename);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);


    ShaderProgram prog("vert.glsl", "frag.glsl");
    prog.use();



    GLuint ebo;
    glGenBuffers(1, &ebo);

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);


    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(vao);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW); 

        GLuint pos_loc = prog.get_attrib_loc("pos");
        glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(pos_loc);

        GLuint tex_loc = prog.get_attrib_loc("tex_coords");
        glVertexAttribPointer(tex_loc, 2, GL_FLOAT, false, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, m_tex_coords)));
        glEnableVertexAttribArray(tex_loc);

        GLuint col_loc = prog.get_attrib_loc("col");
        glVertexAttribPointer(col_loc, 3, GL_FLOAT, false, sizeof(Vertex),
                              reinterpret_cast<void*>(offsetof(Vertex, m_color)));
        glEnableVertexAttribArray(col_loc);

        prog.use();
        glBindVertexArray(vao);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        process_inputs(window, vertices.data(), vertices.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
