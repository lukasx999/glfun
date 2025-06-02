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
    Shader(const char *vert, const char *frag);
    ~Shader();

    Shader &use();
    [[nodiscard]] GLuint get_attrib_loc(const char *name) const;
    // make sure to use() before setting uniforms
    Shader &set_uniform_int(const char *name, int value);
    Shader &set_uniform_float(const char *name, float value);
    Shader &set_uniform_3f(const char *name, glm::vec3 value);
    Shader &set_uniform_mat4(const char *name, glm::mat4 value);

private:
    std::string read_entire_file(const char *filename);
    GLuint setup_shader(GLenum type, const char *filename);

};
