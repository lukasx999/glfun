#include <print>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "texture.hh"



// resize params are unused if one is 0
Texture::Texture(
    GLenum unit,
    const char *filename,
    bool flip_vert,
    int format,
    int resize_width,
    int resize_height
)
    : m_texture(load_texture(filename, flip_vert, format, resize_width, resize_height))
    , m_unit(unit)
{}

Texture &Texture::bind() {
    glActiveTexture(m_unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    return *this;
}

GLuint Texture::load_texture(
    const char *filename,
    bool flip_vert,
    int format,
    int resize_width,
    int resize_height
) {

    stbi_set_flip_vertically_on_load(flip_vert);

    int width, height, _nr_channels;
    uint8_t *data = stbi_load(filename, &width, &height, &_nr_channels, 0);
    if (data == nullptr)
        std::println("Failed to load image {}", filename);

    if (resize_width && resize_height) {

        void *newdata = stbir_resize_uint8_linear(data, width, height, 0, nullptr, resize_width, resize_height, 0, STBIR_RGB);
        if (newdata == nullptr)
            std::println("Failed to resize image");

        width = resize_width;
        height = resize_height;

        stbi_image_free(data);
        data = static_cast<uint8_t*>(newdata);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texture;
}

