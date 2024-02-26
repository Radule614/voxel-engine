#pragma once

#include "Core.h"
#include "Window.h"
#include "../Events/Event.h"
#include "../Events/ApplicationEvent.h"
#include "LayerStack.h"

namespace GLCore
{
class Application
{
public:
    Application(const std::string &name = "Voxel Engine", uint32_t width = 1600, uint32_t height = 900);
    virtual ~Application() = default;

    void Run();

    void OnEvent(Event &e);
    void PushLayer(Layer *layer);
    void PushOverlay(Layer *layer);

    inline Window &GetWindow()
    {
        return *m_Window;
    }
    inline static Application &Get()
    {
        return *s_Instance;
    }

private:
    bool OnWindowClose(WindowCloseEvent &e);

private:
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;
    static Application *s_Instance;
};
} // namespace GLCore
