#version 330 core

in vec3 color;
in vec2 tex_coords;

out vec4 fragment;

uniform sampler2D tex_container;
uniform sampler2D tex_face;

void main() {
    // fragment = vec4(color, 1.0f);
    fragment = texture(tex_face, tex_coords) * vec4(color, 1.0f);
}
