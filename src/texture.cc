#include <filesystem>
#include <memory>
#include <print>

// #define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// #define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "texture.hh"



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
{ }

Texture::Texture(
    GLenum unit,
    const char *filename,
    bool flip_vert,
    int format
)
    : m_texture(load_texture(filename, flip_vert, format))
    , m_unit(unit)
{ }

Texture &Texture::bind() {
    glActiveTexture(m_unit);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    return *this;
}

[[nodiscard]] GLuint Texture::create_texture(int format, int width, int height, Texture::StbiData data) const {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data.get()
    );
    glGenerateMipmap(GL_TEXTURE_2D);
    return tex;
}

[[nodiscard]] Texture::ImageData
Texture::load_image(const char *filename, bool flip_vert) const {

    stbi_set_flip_vertically_on_load(flip_vert);

    int width, height, _nr_channels;
    uint8_t *data = stbi_load(filename, &width, &height, &_nr_channels, 0);
    if (data == nullptr)
        std::println("Failed to load image {}", filename);

    return { StbiData(data), width, height };
}

[[nodiscard]] GLuint Texture::load_texture(
    const char *filename,
    bool flip_vert,
    int format,
    int resize_width,
    int resize_height
) const {

    auto [data, width, height] = load_image(filename, flip_vert);

    if (resize_width && resize_height) {

        uint8_t *newdata = stbir_resize_uint8_linear(
            data.get(),
            width,
            height,
            0,
            nullptr,
            resize_width,
            resize_height,
            0,
            STBIR_RGB
        );
        if (newdata == nullptr)
            std::println("Failed to resize image");

        width = resize_width;
        height = resize_height;
        data.reset(newdata);
    }

    return create_texture(format, width, height, std::move(data));
}

[[nodiscard]] GLuint Texture::load_texture(const char *filename, bool flip_vert, int format) const {
    auto [data, width, height] = load_image(filename, flip_vert);
    return create_texture(format, width, height, std::move(data));
}
