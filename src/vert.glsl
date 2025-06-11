#version 330 core

in vec3 a_pos;
in vec2 a_uv;

out vec2 uv;

uniform mat4 u_mat;

void main() {
    gl_Position = vec4(a_pos*0.1, 1.0f) * u_mat;
    uv = a_uv;
}
