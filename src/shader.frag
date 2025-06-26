#version 330 core

in vec2 uv;

out vec4 fragment;
uniform sampler2D tex;

void main() {
    // fragment = vec4(color, 1.0f);
    fragment = texture(tex, uv);
}
