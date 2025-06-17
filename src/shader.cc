#include <filesystem>
#include <string>
#include <fstream>
#include <print>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.hh"



Shader::Shader(const char *filename_vert, const char *filename_frag)
    : m_id(link_shaders(compile_shader(GL_VERTEX_SHADER, filename_vert),
                        compile_shader(GL_FRAGMENT_SHADER, filename_frag)))
{ }

[[nodiscard]] GLuint Shader::get_attrib_loc(const char *name) const {
    return glGetAttribLocation(m_id, name);
}

Shader::~Shader() {
    glDeleteProgram(m_id);
}

Shader &Shader::set_uniform(const char *name, int value) {
    glUniform1i(glGetUniformLocation(m_id, name), value);
    return *this;
}

Shader &Shader::set_uniform(const char *name, float value) {
    glUniform1f(glGetUniformLocation(m_id, name), value);
    return *this;
}

Shader &Shader::set_uniform(const char *name, glm::vec3 value) {
    glUniform3f(glGetUniformLocation(m_id, name), value.x, value.y, value.z);
    return *this;
}

Shader &Shader::set_uniform(const char *name, glm::mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(m_id, name), 1, false, glm::value_ptr(value));
    return *this;
}

Shader &Shader::use() {
    glUseProgram(m_id);
    return *this;
}

[[nodiscard]]
std::string Shader::read_entire_file(const char *filename) const {
    std::ifstream file(filename);
    return std::string(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );
}

[[nodiscard]] GLuint Shader::link_shaders(GLuint vert, GLuint frag) const {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    glLinkProgram(prog);
    glDeleteShader(vert);
    glDeleteShader(frag);

    int success;
    char info_log[512] = { 0 };
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(m_id, sizeof(info_log), nullptr, info_log);
        std::println(stderr, "Shader Program Linkage failed: {}", info_log);
    }

    return prog;
}

[[nodiscard]]
GLuint Shader::compile_shader(GLenum type, const char *filename) const {
    std::string src = read_entire_file(filename);

    const char *src_raw = src.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src_raw, nullptr);
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
