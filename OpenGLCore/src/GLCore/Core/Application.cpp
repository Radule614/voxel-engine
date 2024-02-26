#include "glpch.h"
#include "Application.h"

#include <glfw/glfw3.h>
#include "Timestep.h"

namespace GLCore
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application *Application::s_Instance = nullptr;
Application::Application(const std::string &name, uint32_t width, uint32_t height)
{
    if (!s_Instance)
        Log::Init();

    GLCORE_ASSERT(!s_Instance, "Application already exists.")
    s_Instance = this;

    m_Window = std::unique_ptr<Window>(Window::Create({name, width, height}));
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
}

void Application::PushLayer(Layer *layer)
{
    m_LayerStack.PushLayer(layer);
}

void Application::PushOverlay(Layer *layer)
{
    m_LayerStack.PushOverlay(layer);
}

void Application::OnEvent(Event &e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled)
            break;
    }
}

void Application::Run()
{
    while (m_Running)
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        for (Layer *layer : m_LayerStack)
            layer->OnUpdate(timestep);

        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent &e)
{
    m_Running = false;
    return true;
}
} // namespace GLCore
