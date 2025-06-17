#include "vertexarray.hh"
#include "vertex.hh"



VertexArray::VertexArray() : m_offset(0) {
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

VertexArray &VertexArray::bind() {
    glBindVertexArray(m_id);
    return *this;
}

VertexArray &VertexArray::unbind() {
    glBindVertexArray(0);
    return *this;
}

template<>
VertexArray &VertexArray::push<float>(GLuint location, GLint components) {
    push_attr(location, components, GL_FLOAT, sizeof(float));
    return *this;
}

void VertexArray::push_attr(GLuint location, GLint components, GLenum type, size_t elem_size) {
    bind();

    glVertexAttribPointer(
        location,
        components,
        type,
        false,
        sizeof(Vertex),
        reinterpret_cast<void*>(m_offset)
    );

    glEnableVertexAttribArray(location);
    m_offset += elem_size * components;
}
