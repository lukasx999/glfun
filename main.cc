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
};

glm::vec3 color_to_vec3(Color color) {
    switch (color) {
        case Color::RED:   return glm::vec3(1.0f, 0.0f, 0.0f);
        case Color::BLUE:  return glm::vec3(0.0f, 0.0f, 1.0f);
        case Color::GREEN: return glm::vec3(0.0f, 1.0f, 0.0f);
        default:           assert(!"unknown color");
    }
}

struct Vertex {
public:
    glm::vec3 m_pos;
    glm::vec3 m_color;

    Vertex(float x, float y, float z, Color color)
    : m_pos(x, y, z)
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

    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        for (size_t i=0; i < v_size; ++i) {
            vertices[i].rotate(0.01f, { 1.0f, 1.0f, 0.0f });
        }
    }

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

class IShape {
public:
    virtual std::vector<Vertex> extract_vertices() const = 0;
};

struct Triangle : public IShape {

    std::array<Vertex, 3> m_vertices;

public:

    Triangle(std::array<Vertex, 3> vertices) : m_vertices(vertices) {}

    Triangle(Color all) : Triangle() {
        for (auto &v : m_vertices)
            v.m_color = color_to_vec3(all);
    }

    Triangle(Color a, Color b, Color c) : Triangle() {
        m_vertices[0].m_color = color_to_vec3(a);
        m_vertices[1].m_color = color_to_vec3(b);
        m_vertices[2].m_color = color_to_vec3(c);
    }

    Triangle() : m_vertices({
        Vertex( 0.5f,  0.5f, 0.0f, Color::RED),  // top-right
        Vertex( 0.5f, -0.5f, 0.0f, Color::BLUE), // bottom-right
        Vertex(-0.5f,  0.5f, 0.0f, Color::GREEN) // top-left
    }) {}

    Triangle &rotate(float angle, glm::vec3 normal) {
        for (auto &v : m_vertices)
            v.rotate(angle, normal);

        return *this;
    }

    virtual std::vector<Vertex> extract_vertices() const {
        return std::vector(m_vertices.begin(), m_vertices.end());
    }

};

struct Rectangle : public IShape {

    std::array<Triangle, 2> m_triangles;

public:

    Rectangle() : m_triangles(
        { Triangle(), Triangle()
            .rotate(180.0f, { 0.0f, 0.0f, 1.0f }) }
    ) {}

    virtual std::vector<Vertex> extract_vertices() const {

        std::vector<Vertex> v;

        for (auto &t : m_triangles)
            for (auto &x : t.extract_vertices())
                v.push_back(x);

        return v;
    }

};

struct VertexBuffer {

    std::vector<Vertex> m_vertices;

public:

    Vertex *data() {
        return m_vertices.data();
    }

    size_t size() const {
        return m_vertices.size();
    }

    void append(const IShape &shape) {
        auto other = shape.extract_vertices();
        m_vertices.insert(m_vertices.end(), other.begin(), other.end());
    }

    void scale(float factor) {
        for (auto &v : m_vertices)
            v.m_pos *= factor;
    }

};


int main() {

    VertexBuffer vbuf;

    vbuf.append(Rectangle());
    // vbuf.append(Triangle());
    //
    // vbuf.append(
    //     Triangle(Color::BLUE)
    //     .rotate(180.0f, { 0.0f, 0.0f, 1.0f })
    // );
    //
    // vbuf.append(
    //     Triangle(Color::RED)
    //     .rotate(90.0f, { 1.0f, 0.0f, 0.0f })
    // );

    vbuf.scale(0.5f);



    GLFWwindow *window = setup_window();

    ShaderProgram prog("vert.glsl", "frag.glsl");
    prog.use();

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);


    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    while (!glfwWindowShouldClose(window)) {

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(
            GL_ARRAY_BUFFER,
            vbuf.size() * sizeof(Vertex),
            vbuf.data(),
            GL_STATIC_DRAW
        );


        GLuint pos_loc = prog.get_attrib_loc("pos");
        glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(pos_loc);

        GLuint col_loc = prog.get_attrib_loc("col");
        glVertexAttribPointer(
            col_loc,
            3,
            GL_FLOAT,
            false,
            sizeof(Vertex),
            reinterpret_cast<void*>(offsetof(Vertex, m_color))
        );
        glEnableVertexAttribArray(col_loc);



        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, vbuf.size());

        process_inputs(window, vbuf.data(), vbuf.size());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
