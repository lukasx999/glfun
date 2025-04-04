#pragma once

#include <print>
#include <span>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include "vertex.hh"


static size_t get_glenum_size(GLenum type) {
    switch (type) {
        case GL_FLOAT:
        return sizeof(float);
    };
}

struct VertexArray {

    GLuint m_id;
    size_t m_offset;

    VertexArray() : m_offset(0) {
        glGenVertexArrays(1, &m_id);
    }

    ~VertexArray() {
        glDeleteVertexArrays(1, &m_id);
    }

    void add_attr(GLuint loc, GLint size, GLenum type) {
        bind();
        glVertexAttribPointer(loc, size, type, false, sizeof(Vertex), reinterpret_cast<void*>(m_offset));
        glEnableVertexAttribArray(loc);
        m_offset += sizeof(float) * size;
    }

    VertexArray &bind() {
        glBindVertexArray(m_id);
        return *this;
    }

    VertexArray &unbind() {
        glBindVertexArray(0);
        return *this;
    }

};



struct IndexBuffer {

    GLuint m_id;
    size_t m_count;

    IndexBuffer(std::span<unsigned int> indices) : m_count(indices.size()) {
        glGenBuffers(1, &m_id);
        bind();
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            m_count * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW
        );
    }

    ~IndexBuffer() {
        glDeleteBuffers(1, &m_id);
    }

    IndexBuffer &bind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
        return *this;
    }

    IndexBuffer &unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        return *this;
    }

};



struct VertexBuffer {

    GLuint m_id;

    VertexBuffer(std::span<Vertex> vertices) {
        glGenBuffers(1, &m_id);
        bind();
        glBufferData(
            GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW
        );
    }

    ~VertexBuffer() {
        glDeleteBuffers(1, &m_id);
    }

    VertexBuffer &bind() {
        glBindBuffer(GL_ARRAY_BUFFER, m_id);
        return *this;
    }

    VertexBuffer &unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return *this;
    }

};

class Texture {

    GLuint m_texture;
    GLenum m_unit;

public:
    // resize params are unused if one is 0
    Texture(GLenum unit, const char *filename, bool flip_vert, int format, int resize_width, int resize_height)
        : m_texture(load_texture(filename, flip_vert, format, resize_width, resize_height))
        , m_unit(unit)
    {}

    Texture &bind() {
        glActiveTexture(m_unit);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        return *this;
    }

private:
    GLuint load_texture(const char *filename, bool flip_vert, int format, int resize_width, int resize_height) {

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

};
