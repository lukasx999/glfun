#include "vertexbuffer.hh"



VertexBuffer::VertexBuffer(std::span<Vertex> vertices) {
    glGenBuffers(1, &m_id);
    bind();
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_id);
}

VertexBuffer &VertexBuffer::bind() {
    glBindBuffer(GL_ARRAY_BUFFER, m_id);
    return *this;
}

VertexBuffer &VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return *this;
}
