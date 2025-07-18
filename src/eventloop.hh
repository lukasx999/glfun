#pragma once

#include <functional>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "glad/gl.h"



class EventLoop {
    GLFWwindow* m_window;
    double m_dt = 0.0f;
    double m_last_frame = 0.0f;
    std::function<void(GLFWwindow*, double)> m_callback;

public:
    EventLoop(GLFWwindow* window, decltype(m_callback) callback)
        : m_window(window)
        , m_callback(callback)
    { }

    void run() {

        while (!glfwWindowShouldClose(m_window)) {

            double time = glfwGetTime();
            m_dt = time - m_last_frame;
            m_last_frame = time;

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            m_callback(m_window, m_dt);

            glfwSwapBuffers(m_window);
            glfwPollEvents();
        }

    }

};
