#pragma once

#include <string>
#include <span>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>



class Texture {
    GLuint m_texture;
    GLenum m_unit;

public:
    // resize params are unused if one is 0
    Texture(
        GLenum unit,
        const char *filename,
        bool flip_vert,
        int format,
        int resize_width,
        int resize_height
    );
    Texture &bind();

private:
    GLuint load_texture(
        const char *filename,
        bool flip_vert,
        int format,
        int resize_width,
        int resize_height
    );

};
