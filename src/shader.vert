#version 330 core

in vec3 a_pos;
in vec2 a_uv;

out vec2 uv;

uniform mat4 u_mvp;

void main() {
    gl_Position = u_mvp * vec4(a_pos, 1.0f);

    uv = a_uv;
}
