#include <string>
#include <iostream>
#include <fstream>
#include <print>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "shader.hh"


ShaderProgram::ShaderProgram(const char *filename_vert, const char *filename_frag)
: m_id(setup_program(filename_vert, filename_frag))
{}

ShaderProgram &ShaderProgram::set_uniform_int(const char *name, int value) {
    int loc = glGetUniformLocation(m_id, name);
    glUniform1i(loc, value);
    return *this;
}

GLuint ShaderProgram::get_attrib_loc(const char *name) {
    return glGetAttribLocation(m_id, name);
}

ShaderProgram &ShaderProgram::use() {
    glUseProgram(m_id);
    return *this;
}

std::string ShaderProgram::read_entire_file(const char *filename) {
    std::ifstream file(filename);
    return std::string(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );
}

GLuint ShaderProgram::setup_shader(GLenum type, const char *filename) {
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

GLuint ShaderProgram::setup_program(const char *file_vert, const char *file_frag) {

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
