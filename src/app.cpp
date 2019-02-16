//
// Created by Göksu Güvendiren on 2019-01-26.
//
#include <glad/glad.h>

#include <iostream>
#include <app.hpp>

grt::app::app(unsigned int width, unsigned int height)// : current_framebuffer()
{
}

grt::window& grt::app::CreateWindow(unsigned int width, unsigned int height, const std::string &window_name)
{
    window = glfw.CreateWindow(width, height, window_name, nullptr, nullptr);
    return window;
}

void grt::app::SetCurrentContext(const grt::window &window)
{
    glfw.SetCurrentContext(window);
}

void grt::app::Render()
{
    glfw.SwapBuffers(window);
}

void grt::app::SetShouldClose(bool close)
{
    glfw.SetShouldClose(window, close);
}

void grt::app::Initialize()
{
    window.AddCallback(std::ref(handler));

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        throw std::runtime_error("Failed to initialize GLAD\n");
    }

    SetFBSize();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void grt::app::SetFBSize()
{
    glfw.SetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    });

}

void grt::app::UseProgram(const program &program)
{
    glUseProgram(program.GetProgram());
}

void grt::app::AddHandler(int key, std::function<void()> function)
{
    handler.Add(key, std::move(function));
}
//
//std::unique_ptr<grt::texture> grt::app::ShadowPass(const grt::point_light &light)
//{
//    grt::texture texture(1024, 1024, grt::texture::type::depth);
////    grt::framebuffer framebuffer()
//}
