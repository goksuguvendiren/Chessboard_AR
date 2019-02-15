//
// Created by Göksu Güvendiren on 2019-01-27.
//

#pragma once

#include <vector>
#include <functional>

#include <GLFW/glfw3.h>

namespace grt
{
    class window
    {
        GLFWwindow* win;
        std::vector<std::function<void(const window&)>> window_callbacks;

    public:
        window() : win(nullptr) {}
        explicit window(GLFWwindow* wind) : win(wind) {}
        window(const grt::window& rhs) = delete;

        void AddCallback(std::function<void(const grt::window&)> callback) { window_callbacks.push_back(std::move(callback)); }
        void CallCallbacks()
        {
            for (auto& callback : window_callbacks)
            {
                callback(*this);
            }
        }

        GLFWwindow* Get() const { return win; }
    };
}