#include <print>
#include <fstream>


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

#include "shader.hh"

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"




#define ARRAY_LEN(xs) (sizeof(xs) / sizeof *(xs))

constexpr const char *shader_vert = "vert.glsl";
constexpr const char *shader_frag = "frag.glsl";
constexpr int width = 1600;
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
    glm::vec3 m_pos;
    glm::vec3 m_color;
public:
    Vertex(float x, float y, float z, Color color)
    : m_pos(x, y, z)
    , m_color(color_to_vec3(color))
    {}
};

void rotate(Vertex *v, size_t size) {
    for (size_t i=0; i < size; ++i) {
        glm::vec3 &pos = v[i].m_pos;

        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, 0.01f, glm::vec3(1.0, 1.0, 0.0));
        pos = glm::vec3(rotationMat * glm::vec4(pos, 1.0));
    }
}

bool is_key_pressed(GLFWwindow *window, int key) {

    static bool old = false;
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;

    bool ret = !old && pressed;

    old = pressed;
    return ret;
}

void process_inputs(GLFWwindow *window, Vertex *vertices, size_t v_size) {

    // if (is_key_pressed(window, GLFW_KEY_J))
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        rotate(vertices, v_size);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}


int main() {

    Vertex vertices[] = {
        Vertex( 0.5f,  0.5f, 0.0f, Color::RED),   // top-right
        Vertex( 0.5f, -0.5f, 0.0f, Color::BLUE),  // bottom-right
        Vertex(-0.5f,  0.5f, 0.0f, Color::GREEN), // top-left

        Vertex( 0.5f, -0.5f, 0.0f, Color::BLUE),  // bottom-right
        Vertex(-0.5f, -0.5f, 0.0f, Color::RED),   // bottom-left
        Vertex(-0.5f,  0.5f, 0.0f, Color::GREEN), // top-left
    };

    for (size_t i=12; i < ARRAY_LEN(vertices); ++i) {
        glm::vec3 &pos = vertices[i].m_pos;

        glm::mat4 rotationMat(1);
        rotationMat = glm::rotate(rotationMat, 90.0f, glm::vec3(0.0, 1.0, 0.0));
        pos = glm::vec3(rotationMat * glm::vec4(pos, 1.0));
    }

    // scale down
    for (size_t i=0; i < ARRAY_LEN(vertices); ++i) {
        glm::vec3 &pos = vertices[i].m_pos;
        pos = pos * 0.5f;
    }



    GLFWwindow *window = setup_window();
    auto prog = ShaderProgram(shader_vert, shader_frag);

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    prog.use();
    glBindVertexArray(vao);

    while (!glfwWindowShouldClose(window)) {

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        int pos_loc = prog.get_attrib_loc("pos");
        glVertexAttribPointer(pos_loc, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
        glEnableVertexAttribArray(pos_loc);

        int col_loc = prog.get_attrib_loc("col");
        glVertexAttribPointer(col_loc, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, m_color)));
        glEnableVertexAttribArray(col_loc);


        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        process_inputs(window, vertices, ARRAY_LEN(vertices));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
