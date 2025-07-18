#pragma once

#include <string>

#include "glad/gl.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>



class Shader {
    GLuint m_id;

public:
    Shader(const char *filename_vert, const char *filename_frag);

    ~Shader();
    Shader(Shader const&) = delete;
    Shader& operator=(Shader const&) = delete;

    Shader &use();
    [[nodiscard]] GLuint get_attrib_loc(const char *name) const;
    Shader &set_uniform(const char *name, int value);
    Shader &set_uniform(const char *name, float value);
    Shader &set_uniform(const char *name, glm::vec3 value);
    Shader &set_uniform(const char *name, glm::mat4 value);

private:
    [[nodiscard]] static GLuint link_shaders(GLuint vert, GLuint frag);
    [[nodiscard]] static GLuint compile_shader(GLenum type, const char *filename);

};
