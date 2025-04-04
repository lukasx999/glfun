#include <print>
#include <array>
#include <cassert>
#include <span>


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"

#include "shader.hh"
#include "vertex.hh"
#include "texture.hh"
#include "vertexarray.hh"
#include "vertexbuffer.hh"
#include "indexbuffer.hh"

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


bool is_key_pressed(GLFWwindow *window, int key) {

    static bool old = false;
    bool pressed = glfwGetKey(window, key) == GLFW_PRESS;
    bool ret = !old && pressed;
    old = pressed;
    return ret;
}

void process_inputs(GLFWwindow *window) {
    static bool mode = false;

    if (is_key_pressed(window, GLFW_KEY_K))
        mode = !mode;

    if (mode)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
}



int main() {

    std::array vertices {
        Vertex({  0.5f,  0.5f, 0.0f }, { 1.0f, 1.0f }, Color::RED),   // top right
        Vertex({  0.5f, -0.5f, 0.0f }, { 1.0f, 0.0f }, Color::BLUE),  // bottom right
        Vertex({ -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f }, Color::GREEN), // bottom left
        Vertex({ -0.5f,  0.5f, 0.0f }, { 0.0f, 1.0f }, Color::CYAN),  // top left
    };

    std::array<unsigned int, 6> indices {
        0, 1, 3, // first triangle
        1, 2, 3, // second triangle
    };


    GLFWwindow *window = setup_window();

    Shader shader("vert.glsl", "frag.glsl");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Texture tex_container(GL_TEXTURE0, "./assets/container.jpg",   false, GL_RGB, 0, 0);
    Texture tex_face     (GL_TEXTURE1, "./assets/awesomeface.png", true,  GL_RGBA, 0, 0);

    shader
        .use()
        .set_uniform_int("tex_container", 0)
        .set_uniform_int("tex_face", 1);

    VertexArray va;
    VertexBuffer vb(vertices);
    IndexBuffer ib(indices);

    GLuint pos = shader.get_attrib_loc("a_pos");
    GLuint tex = shader.get_attrib_loc("a_tex_coords");
    GLuint col = shader.get_attrib_loc("a_col");
    va
        .push_attr(pos, 3, GL_FLOAT)
        .push_attr(tex, 2, GL_FLOAT)
        .push_attr(col, 3, GL_FLOAT);



    while (!glfwWindowShouldClose(window)) {

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        double x = glm::sin(glfwGetTime() * 2.0f) / 2.0f;
        float y = glm::sin(glfwGetTime());
        glm::mat4 mat(1.0f);
        mat = glm::rotate(mat, y * glm::radians(360.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        mat = glm::scale(mat, glm::vec3(x * .25 + .5));

        shader.set_uniform_3f("u_position", glm::vec3(x, x, .0f));
        shader.set_uniform_mat4("u_rot_mat", mat);


        tex_container.bind();
        tex_face.bind();
        shader.use();
        ib.bind();
        va.bind();
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);

        process_inputs(window);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;

}
