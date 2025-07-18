#include "obj.hh"
#include <print>
#include <unordered_map>
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
#include "camera.hh"
#include "renderer.hh"

#include "glad/gl.h"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>







[[nodiscard]] static GLFWwindow* setup_glfw() {

    glfwSetErrorCallback([]([[maybe_unused]] int error_code, const char* description) {
        std::println(stderr, "GLFW Error: {}", description);
    });

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL", nullptr, nullptr);
    assert(window != nullptr);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    gladLoadGL(glfwGetProcAddress);

    glfwSetFramebufferSizeCallback(
        window, []([[maybe_unused]] GLFWwindow* win, int w, int h) {
            glViewport(0, 0, w, h);
        }
    );

    return window;
}

using GLFWKey = int;

[[nodiscard]] static bool is_key_rising(GLFWwindow* window, GLFWKey key) {
    static bool old = false;
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool ret = !old && pressed;
    old = pressed;
    return ret;
}

[[nodiscard]] static bool is_key_down(GLFWwindow* window, GLFWKey key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

static void process_inputs(GLFWwindow* window, State& state, float dt) {

    if (is_key_rising(window, GLFW_KEY_E)) {
        state.polygon_mode = !state.polygon_mode;
    }

    glPolygonMode(GL_FRONT_AND_BACK, state.polygon_mode ? GL_LINE : GL_FILL);

    if (is_key_down(window, GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(window, 1);

    if (is_key_down(window, GLFW_KEY_W)) state.cam.move_forward(dt);
    if (is_key_down(window, GLFW_KEY_A)) state.cam.move_left(dt);
    if (is_key_down(window, GLFW_KEY_S)) state.cam.move_backward(dt);
    if (is_key_down(window, GLFW_KEY_D)) state.cam.move_right(dt);

}

[[nodiscard]] auto glstr_to_cstr(const GLubyte* glstr) {
    return reinterpret_cast<const char*>(glstr);
}

void print_useful_info() {
    std::println("vendor: {}", glstr_to_cstr(glGetString(GL_VENDOR)));
    std::println("version: {}", glstr_to_cstr(glGetString(GL_VERSION)));
    std::println("renderer: {}", glstr_to_cstr(glGetString(GL_RENDERER)));
    std::println("shading language version: {}", glstr_to_cstr(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    std::println();
}

[[nodiscard]] constexpr auto gl_debug_type_to_cstr(GLenum type) {
    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            return "ERROR";
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            return "DEPRECATED_BEHAVIOR";
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            return "UNDEFINED_BEHAVIOR";
        case GL_DEBUG_TYPE_PORTABILITY:
            return "PORTABILITY";
        case GL_DEBUG_TYPE_PERFORMANCE:
            return "PERFORMANCE";
        case GL_DEBUG_TYPE_OTHER:
            return "OTHER";
    }
    std::unreachable();
}

[[nodiscard]] constexpr auto gl_debug_severity_to_cstr(GLenum severity) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_LOW:
            return "LOW";
        case GL_DEBUG_SEVERITY_MEDIUM:
            return "MEDIUM";
        case GL_DEBUG_SEVERITY_HIGH:
            return "HIGH";
    }
    std::unreachable();
}

static void with_opengl_context(std::function<void(GLFWwindow*)> fn) {
    GLFWwindow *window = setup_glfw();
    fn(window);
    glfwDestroyWindow(window);
    glfwTerminate();
}


int main() {

    State state;

    std::ifstream file("./backpack/backpack.obj");
    std::string obj_src((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Parser parser(obj_src);
    auto vertices = parser.parse();

    with_opengl_context([&](GLFWwindow* window) {

        // IMGUI_CHECKVERSION();
        // ImGui::CreateContext();
        // ImGuiIO& io = ImGui::GetIO();
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        // ImGui_ImplGlfw_InitForOpenGL(window, true);
        // ImGui_ImplOpenGL3_Init();

        glDebugMessageCallback([](
            [[maybe_unused]] GLenum src,
            [[maybe_unused]] GLenum type,
            [[maybe_unused]] GLuint id,
            [[maybe_unused]] GLenum severity,
            [[maybe_unused]] GLsizei len,
            const char *msg,
            [[maybe_unused]] const void *args
        ) {
                               std::println(stderr, "> OpenGL Error:");
                               std::println(stderr, "Type: {}", gl_debug_type_to_cstr(type));
                               std::println(stderr, "Severity: {}", gl_debug_severity_to_cstr(severity));
                               std::println(stderr, "Message: {}", msg);
                               std::println(stderr);
                               }, nullptr);

        print_useful_info();

        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Texture texture(GL_TEXTURE0, "./assets/awesomeface.png", false, GL_RGBA);
        Texture texture(GL_TEXTURE0, "./backpack/diffuse.jpg", false, GL_RGB);

        double dt = 0.0f;
        double last_frame = 0.0f;

        glfwSetWindowUserPointer(window, &state);

        glfwSetCursorPosCallback(window, [](GLFWwindow *win, double x, double y) {
            static glm::vec2 old(0.0f);

            auto *state_ptr = static_cast<State*>(glfwGetWindowUserPointer(win));
            assert(state_ptr != nullptr);
            auto &state = *state_ptr;

            glm::vec2 now(x, y);

            auto delta = now - old;
            state.cam.rotate(delta);
            old = now;
        });

        glfwSetScrollCallback(window, [](GLFWwindow* win, [[maybe_unused]] double x, double y) {
            float scroll_factor = 10.0f;
            float max_fov = 90.0f;

            auto* state_ptr = static_cast<State*>(glfwGetWindowUserPointer(win));
            assert(state_ptr != nullptr);
            auto& state = *state_ptr;

            state.fov_deg -= y * scroll_factor;
            state.fov_deg = std::clamp(state.fov_deg, 1.0f, max_fov);
        });

        Renderer rd(vertices);

        while (!glfwWindowShouldClose(window)) {

            // ImGui_ImplOpenGL3_NewFrame();
            // ImGui_ImplGlfw_NewFrame();
            // ImGui::NewFrame();
            // ImGui::ShowDemoWindow();

            double time = glfwGetTime();
            dt = time - last_frame;
            last_frame = time;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            rd.render(texture, state, { 0.0f,  0.0f,  0.0f });

            // ImGui::Render();
            // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            process_inputs(window, state, dt);
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // ImGui_ImplOpenGL3_Shutdown();
        // ImGui_ImplGlfw_Shutdown();
        // ImGui::DestroyContext();

    });


    return EXIT_SUCCESS;
}
