//
// Created by Göksu Güvendiren on 2019-01-26.
//

#pragma once

#include <optional>
#include <lights/point_light.hpp>

#include "glfw.hpp"
#include "program.hpp"
#include "utils.hpp"
//#include "texture.hpp"

namespace grt
{
    class app
    {
        grt::window window;
        grt::glfw_object glfw;
        cc::utils::key_handlers handler;

    public:
        app(unsigned int width, unsigned int height);

        void Render();
//        std::unique_ptr<grt::texture> ShadowPass(const grt::point_light& light);
        void SetShouldClose(bool close);
        void Initialize();
        void SetFBSize();

        void UseProgram(const program &program);

        bool Alive() { return !glfw.ShouldClose(window); }

        grt::window& CreateWindow(unsigned int width, unsigned int height, const std::string &window_name);
        void SetCurrentContext(const grt::window& window);

        void AddHandler(int key, std::function<void()> function);
    };
}