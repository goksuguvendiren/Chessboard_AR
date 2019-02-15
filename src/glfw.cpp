//
// Created by Göksu Güvendiren on 2019-01-19.
//

#include <glfw.hpp>
#include <iostream>

grt::glfw_object::glfw_object()
{
    auto result = glfwInit();

    if (result == GLFW_FALSE)
    {
        std::cerr << "glfw not initialized \n";
        return;
    }

    glfwSetErrorCallback([](int error, const char* description)
    {
        std::cerr << "Error : " << description << '\n';
    });

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#endif
}

grt::glfw_object::~glfw_object()
{
    std::cerr << "Terminate called \n";
    glfwTerminate();
}

grt::window grt::glfw_object::CreateWindow(unsigned int width, unsigned int height, const std::string &window_name,
                                    GLFWmonitor *monitor, GLFWwindow *share)
{
    return grt::window(glfwCreateWindow(width, height, window_name.c_str(), monitor, share));
}

static std::function<void(int, const char*)> err_handle;
void grt::glfw_object::SetErrorFunction(std::function<void(int, const char*)> err_function)
{
    err_handle = std::move(err_function);
    glfwSetErrorCallback([](int i, const char* str){
        err_handle(i, str);
    });
}

static std::function<void(GLFWwindow *window, int, int)> fbs_handle;
void grt::glfw_object::SetFramebufferSizeCallback(const grt::window& window, std::function<void(GLFWwindow *window, int width,
                                                                                         int height)> callback)
{
    fbs_handle = std::move(callback);
    glfwSetFramebufferSizeCallback(window.Get(), [](GLFWwindow* win, int w, int h)
    {
        fbs_handle(win, w, h);
    });
}

void grt::glfw_object::SetCurrentContext(const grt::window& window)
{
    glfwMakeContextCurrent(window.Get());
}

void grt::glfw_object::SwapBuffers(const grt::window &window)
{
    glfwSwapBuffers(window.Get());
}

void grt::glfw_object::PollEvents()
{
    glfwPollEvents();
}

int grt::glfw_object::ShouldClose(const grt::window &window)
{
    return glfwWindowShouldClose(window.Get());
}

void grt::glfw_object::SetShouldClose(const grt::window &window, bool close)
{
    glfwSetWindowShouldClose(window.Get(), close);
}
