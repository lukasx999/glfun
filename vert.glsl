#version 330 core

in vec3 a_pos;
in vec3 a_col;
in vec2 a_tex_coords;

out vec3 color;
out vec2 tex_coords;

void main() {
    gl_Position = vec4(a_pos, 1.0f);
    color = a_col;
    tex_coords = a_tex_coords;
}
