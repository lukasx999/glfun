#version 330 core

in vec3 a_pos;
in vec2 a_uv;

out vec2 uv;

uniform mat4 u_mat;
uniform mat4 u_transform;
uniform float u_zoom;

void main() {
    // gl_Position = vec4(a_pos * u_zoom, 1.0f) * u_mat;
    gl_Position = vec4(a_pos * u_zoom, 1.0f) * u_mat * u_transform;
    uv = a_uv;
}
