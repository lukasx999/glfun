#pragma once

#include <string>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>



class ShaderProgram {
private:
    GLuint m_program;

public:
    ShaderProgram(const char *filename_vert, const char *filename_frag);
    void use();
    GLuint get_attrib_loc(const char *name);
    // make sure to use() the program before setting uniforms
    void set_uniform_int(const char *name, int value);

private:
    std::string read_entire_file(const char *filename);
    GLuint setup_shader(GLenum type, const char *filename);
    GLuint setup_program(const char *file_vert, const char *file_frag);
};
