//
// Created by Göksu Güvendiren on 2019-01-19.
//

#pragma once

#include <GLFW/glfw3.h>
#include <functional>
#include <vector>

#include "window.hpp"

namespace grt
{
    class glfw_object
    {
    public:
        glfw_object();
        ~glfw_object();
        void SetCurrentContext(const grt::window& window);

        void SetErrorFunction(std::function<void(int, const char*)> err_function);
        void SetFramebufferSizeCallback(const grt::window& window, std::function<void(GLFWwindow* window, int width, int height)> callback);

//        void AddFunction(std::function<void(grt::window)> callback) { window.AddCallback(std::move(callback)); }

        void SwapBuffers(const grt::window& window);
        int ShouldClose(const grt::window &window);
        void PollEvents();
        void SetShouldClose(const grt::window& window, bool close);

        grt::window CreateWindow(unsigned int width, unsigned int height, const std::string& window_name, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);

//        bool RenderLoop();
    };
}