#include "indexbuffer.hh"


IndexBuffer::IndexBuffer(std::span<unsigned int> indices) : m_count(indices.size()) {
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        m_count * sizeof(unsigned int),
        indices.data(),
        GL_STATIC_DRAW
    );
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_id);
}

IndexBuffer &IndexBuffer::bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
    return *this;
}

IndexBuffer &IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return *this;
}
