#include <print>
#include <vector>
#include <regex>
#include <array>
#include <ranges>
#include <cassert>
#include <memory>
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


#define PRINT(x) std::println("{}: {}", #x, x)



static constexpr int WIDTH  = 1600;
static constexpr int HEIGHT = 900;

struct State {
    float u_zoom = 0.1;
    glm::mat4 u_mat { 1.0f };
    bool polygon_mode = false;
};

[[nodiscard]] static GLFWwindow *setup_glfw() {

    glfwSetErrorCallback([]([[maybe_unused]] int error_code, const char* description) {
        std::println(stderr, "GLFW Error: {}", description);
    });

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Cube", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL(glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(
        window,
        []([[maybe_unused]] GLFWwindow *win, int w, int h) {
            glViewport(0, 0, w, h);
        }
    );

    return window;
}

using GLFWKey = int;

[[nodiscard]] static bool is_key_rising(GLFWwindow *window, GLFWKey key) {
    static bool old = false;
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool ret = !old && pressed;
    old = pressed;
    return ret;
}

[[nodiscard]] static bool is_key_down(GLFWwindow *window, GLFWKey key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

static void process_inputs(GLFWwindow *window, State &state) {

    if (is_key_rising(window, GLFW_KEY_K)) {
        state.polygon_mode = !state.polygon_mode;
    }

    glPolygonMode(GL_FRONT_AND_BACK, state.polygon_mode ? GL_LINE : GL_FILL);

    if (is_key_down(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);

    if (is_key_down(window, GLFW_KEY_RIGHT))
        state.u_mat = glm::rotate(state.u_mat, glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (is_key_down(window, GLFW_KEY_LEFT))
        state.u_mat = glm::rotate(state.u_mat, -glm::radians(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    if (is_key_down(window, GLFW_KEY_UP))
        state.u_mat = glm::rotate(state.u_mat, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    if (is_key_down(window, GLFW_KEY_DOWN))
        state.u_mat = glm::rotate(state.u_mat, -glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

}


[[nodiscard]] static std::vector<Vertex> parse_obj(const char *filename) {
    std::ifstream file(filename);
    std::string obj((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    std::vector<glm::vec3> tmp_vertices;
    std::vector<glm::vec2> tmp_uv;
    std::vector<glm::vec3> tmp_normal;
    std::vector<unsigned int> vertex_idx, uv_idx, norm_idx;

    std::istringstream stream(obj);

    std::string line;
    while (std::getline(stream, line)) {

        std::smatch matches;
        const std::string re_num(R"([0-9]+)");
        const std::string re_idx(std::format(R"(({0})/({0})/({0}))", re_num));
        const std::string re_comment(R"(\s*#?.*$)");
        const std::string re_float(R"([+-]?[0-9]*\.?[0-9]*)");
        const std::regex re_vertex(std::format(R"(v\s+({0})\s+({0})\s+({0}){1})", re_float, re_comment));
        const std::regex re_tex(std::format(R"(vt\s+({0})\s+({0}){1})", re_float, re_comment));
        const std::regex re_norm(std::format(R"(vn\s+({0})\s+({0})\s+({0}){1})", re_float, re_comment));
        const std::regex re_face(std::format(R"(f\s+{0}\s+{0}\s+{0}{1})", re_idx, re_comment));
        const std::regex re_face_vertex(std::format(R"(f\s+({0})\s+({0})\s+({0}){1})", re_num, re_comment));

        if (std::regex_match(line, matches, re_vertex)) {
            float x = stof(matches.str(1));
            float y = stof(matches.str(2));
            float z = stof(matches.str(3));
            tmp_vertices.push_back({ x, y, z });

        } else if (std::regex_match(line, matches, re_tex)) {
            float u = stof(matches.str(1));
            float v = stof(matches.str(2));
            tmp_uv.push_back({ u, v });

        } else if (std::regex_match(line, matches, re_norm)) {
            float x = stof(matches.str(1));
            float y = stof(matches.str(2));
            float z = stof(matches.str(3));
            tmp_normal.push_back({ x, y, z });

        } else if (std::regex_match(line, matches, re_face)) {
            vertex_idx.push_back(stoi(matches.str(1)));
            uv_idx    .push_back(stoi(matches.str(2)));
            norm_idx  .push_back(stoi(matches.str(3)));
            vertex_idx.push_back(stoi(matches.str(4)));
            uv_idx    .push_back(stoi(matches.str(5)));
            norm_idx  .push_back(stoi(matches.str(6)));
            vertex_idx.push_back(stoi(matches.str(7)));
            uv_idx    .push_back(stoi(matches.str(8)));
            norm_idx  .push_back(stoi(matches.str(9)));

        } else if (std::regex_match(line, matches, re_face_vertex)) {
            vertex_idx.push_back(stoi(matches.str(1)));
            vertex_idx.push_back(stoi(matches.str(2)));
            vertex_idx.push_back(stoi(matches.str(3)));
        }

    }

    std::vector<Vertex> verts;

    // for (auto &&[vert, uv, norm] : std::views::zip(vertex_idx, uv_idx, norm_idx)) {
    //     verts.push_back(Vertex(tmp_vertices[vert-1], tmp_uv[uv-1]));
    // }

    for (auto &v : vertex_idx) {
        verts.push_back(tmp_vertices[v-1]);
    }

    return verts;
}

int main() {

    // std::array vertices {
    //     Vertex({ -0.5f, -0.5f, 0.0f }),
    //     Vertex({  0.5f, -0.5f, 0.0f }),
    //     Vertex({  0.0f,  0.5f, 0.0f })
    // };

    State state;

    auto vertices = parse_obj("./assets/teapot.obj");

    // for (auto &v : vertices) {
    //     std::println("{}, {}, {}", v.m_pos.x, v.m_pos.y, v.m_pos.z);
    // }

    GLFWwindow *window = setup_glfw();
    {

        glDebugMessageCallback([](
            [[maybe_unused]] GLenum src,
            [[maybe_unused]] GLenum type,
            [[maybe_unused]] GLuint id,
            [[maybe_unused]] GLenum severity,
            [[maybe_unused]] GLsizei len,
            const char *msg,
            [[maybe_unused]] const void *args
        ) { std::println(stderr, "OpenGL Error: {}", msg); }, nullptr);

        Shader shader("vert.glsl", "frag.glsl");

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


        shader.use()
              .set_uniform("tex", 0);

        VertexArray va;
        VertexBuffer vb(vertices);

        // Texture texture(GL_TEXTURE0, "./texture.png", false, GL_RGBA);

        GLuint pos = shader.get_attrib_loc("a_pos");
        GLuint uv  = shader.get_attrib_loc("a_uv");
        GLuint col = shader.get_attrib_loc("a_col");

        va.add<float>(pos, 3)
          .add<float>(uv,  2)
          .add<float>(col, 3);

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        glfwSetWindowUserPointer(window, &state);
        glfwSetScrollCallback(window, [](GLFWwindow* window, [[maybe_unused]] double xoffset, double yoffset) {
            auto state = static_cast<State*>(glfwGetWindowUserPointer(window));
            state->u_zoom += yoffset / 100;
        });

        // TODO: font rendering using freetype

        while (!glfwWindowShouldClose(window)) {

            state.u_zoom = std::clamp(state.u_zoom, 0.0f, 1.0f);

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            shader.set_uniform("u_mat", state.u_mat);
            shader.set_uniform("u_zoom", state.u_zoom);

            // texture.bind();
            shader.use();
            va.bind();
            glDrawArrays(GL_TRIANGLES, 0, vertices.size());

            process_inputs(window, state);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
