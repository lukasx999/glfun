#pragma once

#include <string>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>



class ShaderProgram {
private:
    GLuint m_program;
public:
    ShaderProgram(const char *filename_vert, const char *filename_frag)
    : m_program(setup_program(filename_vert, filename_frag))
    {}
    void use();
    GLuint get_attrib_loc(const char *name);

private:
    std::string read_entire_file(const char *filename);
    GLuint setup_shader(GLenum type, const char *filename);
    GLuint setup_program(const char *file_vert, const char *file_frag);
};
