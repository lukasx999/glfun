#version 330 core

in vec3 color;
in vec2 tex_coords;

out vec4 fragment;

uniform sampler2D tex_container;
uniform sampler2D tex_face;
uniform sampler2D tex_logo;

void main() {
    // fragment = vec4(color, 1.0f);
    // fragment = texture(tex, tex_coords) * vec4(color, 1.0f);
    vec4 face = texture(tex_face, tex_coords);
    vec4 container = texture(tex_container, tex_coords);
    vec4 logo = texture(tex_logo, tex_coords);

    // fragment = mix(container, face, 0.2f) * vec4(color, 1.0f);
    fragment = mix(logo, face, 0.2f) * vec4(color, 1.0f);
}
