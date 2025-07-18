#pragma once

#include <filesystem>
#include <string>
#include <span>

#include "stb_image.h"

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
    // unit: GL_TEXTUREn
    Texture(GLenum unit, char const* filename, bool flip_vert, GLenum format);
    Texture(GLenum unit, char const* filename, bool flip_vert, GLenum format, int resize_width, int resize_height);
    Texture& bind();

private:
    using StbiDeleter = decltype([](uint8_t* data) { stbi_image_free(data); });
    using StbiData = std::unique_ptr<uint8_t, StbiDeleter>;
    using ImageData = std::tuple<StbiData, int, int>;

    [[nodiscard]] static ImageData load_image(
        const char *filename,
        bool flip_vert
    );

    [[nodiscard]] static GLuint load_texture(
        const char *filename,
        bool flip_vert,
        GLenum format,
        int resize_width,
        int resize_height
    );

    [[nodiscard]] static GLuint load_texture(
        const char *filename,
        bool flip_vert,
        GLenum format
    );

    [[nodiscard]] static GLuint create_texture(
        GLenum format,
        int width,
        int height,
        StbiData data
    );

};
