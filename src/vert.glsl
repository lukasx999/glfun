#version 330 core

in vec3 a_pos;
in vec2 a_uv;

out vec2 uv;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

uniform mat4 u_mat;
uniform float u_zoom;

void main() {
    // gl_Position = vec4(a_pos * u_zoom, 1.0f) * u_mat;
    gl_Position = u_projection * u_view * u_model * vec4(a_pos, 1.0f);

    uv = a_uv;
}
