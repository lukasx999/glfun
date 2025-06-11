#include <print>
#include <vector>
#include <array>
#include <cassert>
#include <span>
#include <fstream>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "vertex.hh"
#include "vertexarray.hh"
#include "vertexbuffer.hh"
#include "indexbuffer.hh"
#include "shader.hh"
#include "texture.hh"

#include "glad/gl.h"


constexpr int WIDTH  = 1600;
constexpr int HEIGHT = 900;

[[nodiscard]] static GLFWwindow *setup_glfw() {

    glfwSetErrorCallback([](__attribute__((unused)) int error_code, const char* description) {
        std::println(stderr, "GLFW ERROR: {}", description);
    });

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Cube", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL(glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(
        window,
        [](__attribute__((unused)) GLFWwindow *win, int w, int h) {
            glViewport(0, 0, w, h);
        }
    );

    return window;
}

[[nodiscard]] static bool is_key_rising(GLFWwindow *window, int key) {

    static bool old = false;
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool ret = !old && pressed;
    old = pressed;
    return ret;
}

static void process_inputs(GLFWwindow *window) {
    static bool mode = false;

    if (is_key_rising(window, GLFW_KEY_K))
        mode = !mode;

    glPolygonMode(GL_FRONT_AND_BACK, mode ? GL_LINE : GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}

static std::vector<Vertex> parse_obj(const char *filename) {
    std::ifstream file(filename);
    std::string obj(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );

    std::vector<glm::vec3> tmp_vertices;
    std::vector<glm::vec2> tmp_uv;
    std::vector<glm::vec3> tmp_normal;

    std::istringstream stream(obj);

    std::string line;
    while (std::getline(stream, line)) {

        std::vector<std::string> elems;

        std::istringstream linestream(line);
        std::string elem;
        while (std::getline(linestream, elem, ' ')) {
            elems.push_back(elem);
        }

        if (!elems[0].compare("v")) {
            float x = std::stof(elems[1]);
            float y = std::stof(elems[2]);
            float z = std::stof(elems[3]);
            tmp_vertices.push_back({ x, y, z });
        }

        if (!elems[0].compare("vt")) {
            float u = std::stof(elems[1]);
            float v = std::stof(elems[2]);
            tmp_uv.push_back({ u, v });
        }

        if (!elems[0].compare("vn")) {
            float x = std::stof(elems[1]);
            float y = std::stof(elems[2]);
            float z = std::stof(elems[3]);
            tmp_normal.push_back({ x, y, z });
        }

        if (!elems[0].compare("f")) {
            float x = std::stof(elems[1]);
            float y = std::stof(elems[2]);
            float z = std::stof(elems[3]);
            tmp_normal.push_back({ x, y, z });
        }

    }

    std::vector<Vertex> verts;

    for (auto &v : tmp_vertices) {
        verts.push_back(Vertex(v));
    }

    return verts;

}

int main() {

    // std::array vertices {
    //     Vertex({ -0.5f, -0.5f, 0.0f }),
    //     Vertex({  0.5f, -0.5f, 0.0f }),
    //     Vertex({  0.0f,  0.5f, 0.0f })
    // };

    auto vertices = parse_obj("model.obj");
    for (auto &v : vertices) {
        std::println("{}, {}, {}", v.m_pos.x, v.m_pos.y, v.m_pos.z);
    }

    GLFWwindow *window = setup_glfw();
    {

        Shader shader("vert.glsl", "frag.glsl");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        shader.use();

        VertexArray va;
        VertexBuffer vb(vertices);

        GLuint pos = shader.get_attrib_loc("a_pos");
        GLuint tex = shader.get_attrib_loc("a_tex_coords");
        GLuint col = shader.get_attrib_loc("a_col");

        va
            .push_attr(pos, 3, GL_FLOAT)
            .push_attr(tex, 2, GL_FLOAT)
            .push_attr(col, 3, GL_FLOAT);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);


        while (!glfwWindowShouldClose(window)) {

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.use();
            va.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            process_inputs(window);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
