#pragma once

#include <span>

#include "glad/gl.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "vertex.hh"



class Texture {

    GLuint m_texture;
    GLenum m_unit;

public:
    // resize params are unused if one is 0
    Texture(GLenum unit, const char *filename, bool flip_vert, int format, int resize_width, int resize_height);
    Texture &bind();

private:
    GLuint load_texture(const char *filename, bool flip_vert, int format, int resize_width, int resize_height);

};
