#include "glpch.h"
#include "WindowsWindow.h"

#include"GLCore/Events/ApplicationEvent.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace GLCore
{
static bool s_GLFWInitialized = false;
static void GLFWErrorCallback(int error, const char *description)
{
    LOG_ERROR("GLFW Error ({0}): {1}", error, description);
}
Window *Window::Create(const WindowProps &props)
{
    return new WindowsWindow(props);
}
WindowsWindow::WindowsWindow(const WindowProps &props)
{
    Init(props);
}
WindowsWindow::~WindowsWindow()
{
    Shutdown();
}

void WindowsWindow::Init(const WindowProps &props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    if (!s_GLFWInitialized)
    {
        int success = glfwInit();
        GLCORE_ASSERT(success, "Could not initialize GLFW!");
        glfwSetErrorCallback(GLFWErrorCallback);
        s_GLFWInitialized = true;
    }

    m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
    glfwMakeContextCurrent(m_Window);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    GLCORE_ASSERT(status, "Failed to initualize Glad!");

    LOG_INFO("OpenGL info:");
    LOG_INFO("  Vendor: {0}", (char*)glGetString(GL_VENDOR));
    LOG_INFO("  Renderer: {0}", (char*)glGetString(GL_RENDERER));
    LOG_INFO("  Version: {0}", (char*)glGetString(GL_VERSION));

    glfwSetWindowUserPointer(m_Window, &m_Data);
    SetVSync(true);

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow *window, int width, int height) {
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        data.Width = width;
        data.Height = height;
        // WindowResizeEvent event(width, height);
        // data.EventCallback(event);
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *window) {
        WindowData &data = *(WindowData *)glfwGetWindowUserPointer(window);
        WindowCloseEvent event;
        data.EventCallback(event);
    });
}

void WindowsWindow::Shutdown()
{
    glfwDestroyWindow(m_Window);
}

void WindowsWindow::OnUpdate()
{
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
}
void WindowsWindow::SetVSync(bool enabled)
{
    if (enabled)
        glfwSwapInterval(1);
    else
        glfwSwapInterval(0);
    m_Data.VSync = enabled;
}
bool WindowsWindow::IsVSync() const
{
    return m_Data.VSync;
}

} // namespace GLCore
