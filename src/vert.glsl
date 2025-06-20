#version 330 core

in vec3 a_pos;
in vec2 a_uv;

out vec2 uv;

uniform mat4 u_mat;
uniform float u_zoom;

void main() {
    gl_Position = vec4(a_pos * u_zoom, 1.0f) * u_mat;
    uv = a_uv;
}
