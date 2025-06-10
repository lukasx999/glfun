#include "vertexarray.hh"
#include "vertex.hh"
#include <utility>



VertexArray::VertexArray() : m_offset(0) {
    glGenVertexArrays(1, &m_id);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

VertexArray &VertexArray::push_attr(GLuint loc, GLint size, GLenum type) {
    bind();
    glVertexAttribPointer(
        loc,
        size,
        type,
        false,
        sizeof(Vertex),
        reinterpret_cast<void*>(m_offset)
    );
    glEnableVertexAttribArray(loc);
    m_offset += sizeof_gltype(type) * size;
    return *this;
}

VertexArray &VertexArray::bind() {
    glBindVertexArray(m_id);
    return *this;
}

VertexArray &VertexArray::unbind() {
    glBindVertexArray(0);
    return *this;
}

[[nodiscard]] constexpr size_t VertexArray::sizeof_gltype(GLenum type) const {
    switch (type) {
        case GL_FLOAT: return sizeof(float);
        default: assert(!"unknown type");
    };
    std::unreachable();
}
