#include <print>
#include <fstream>
#include <memory>
#include <utility>
#include <vector>

#define GLAD_GL_IMPLEMENTATION
#include "glad/gl.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"



constexpr const char *shader_vert = "vert.glsl";
constexpr const char *shader_frag = "frag.glsl";
constexpr int width = 1600;
constexpr int height = 900;



std::string read_entire_file(const char *filename) {
    std::ifstream file(filename);
    return std::string((std::istreambuf_iterator<char>(file)),
                       (std::istreambuf_iterator<char>()));
}

GLuint setup_shader(GLenum type, const char *filename) {
    std::string shader_src = read_entire_file(filename);
    const char *shader_src_raw = shader_src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_src_raw, nullptr);
    glCompileShader(shader);

    int success;
    char info_log[512] = { 0 };
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);
        std::println(stderr, "{}: Shader Compilation failed: {}", filename, info_log);
    }

    return shader;
}

GLuint setup_program(const char *file_vert, const char *file_frag) {

    GLuint vert = setup_shader(GL_VERTEX_SHADER, file_vert);
    GLuint frag = setup_shader(GL_FRAGMENT_SHADER, file_frag);

    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    glDeleteShader(vert);
    glDeleteShader(frag);

    int success;
    char info_log[512] = { 0 };
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program, sizeof(info_log), nullptr, info_log);
        std::println(stderr, "Shader Program Linkage failed: {}", info_log);
    }

    return program;
}

void process_inputs(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

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



struct Vertex {
    float pos[3];
public:
    Vertex(float x, float y, float z) : pos{x, y, z} {}
};



int main() {

    Vertex vertices[] = {
        Vertex(-0.5f, -0.5f, 0.0f),
        Vertex(0.5f, -0.5f, 0.0f),
        Vertex(0.0f,  0.5f, 0.0f)
    };

    GLFWwindow *window = setup_window();

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(0);


    GLuint program = setup_program(shader_vert, shader_frag);

    while (!glfwWindowShouldClose(window)) {

        process_inputs(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
