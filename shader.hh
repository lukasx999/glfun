#pragma once

#include <string>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>



struct Shader {

    GLuint m_id;

    Shader(const char *vert, const char *frag);
    ~Shader();
    Shader &use();
    GLuint get_attrib_loc(const char *name) const;
    // make sure to use() before setting uniforms
    Shader &set_uniform_int(const char *name, int value);

private:
    std::string read_entire_file(const char *filename);
    GLuint setup_shader(GLenum type, const char *filename);
};
