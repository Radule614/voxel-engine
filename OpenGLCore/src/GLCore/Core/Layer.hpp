#pragma once

#include "Core.hpp"
#include "Timestep.hpp"
#include "../Events/Event.hpp"

namespace GLCore
{
class Layer
{
public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer() = default;
    virtual void OnAttach()
    {
    }
    virtual void OnDetach()
    {
    }
    virtual void OnUpdate(Timestep ts)
    {
    }
    virtual void OnImGuiRender()
    {
    }
    virtual void OnEvent(Event &event)
    {
    }

    inline const std::string &GetName()
    {
        return m_DebugName;
    }

private:
    std::string m_DebugName;
};
} 
