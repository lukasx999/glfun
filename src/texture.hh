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
    // unit is GL_TEXTURE0 for example
    Texture(GLenum unit, const char *filename, bool flip_vert, int format);
    Texture(GLenum unit, const char *filename, bool flip_vert, int format, int resize_width, int resize_height);
    Texture &bind();

private:

    typedef decltype([](uint8_t *data) {
        stbi_image_free(data);
    }) StbiDeleter;
    typedef std::unique_ptr<uint8_t, StbiDeleter> StbiData;
    typedef std::tuple<StbiData, int, int> ImageData;

    [[nodiscard]] ImageData load_image(
        const char *filename,
        bool flip_vert
    ) const;

    [[nodiscard]] GLuint load_texture(
        const char *filename,
        bool flip_vert,
        int format,
        int resize_width,
        int resize_height
    ) const;

    [[nodiscard]] GLuint load_texture(
        const char *filename,
        bool flip_vert,
        int format
    ) const;

    [[nodiscard]] GLuint create_texture(
        int format,
        int width,
        int height,
        StbiData data
    ) const;

};
