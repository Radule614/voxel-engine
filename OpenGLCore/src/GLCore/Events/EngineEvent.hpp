#pragma once

#include "Event.hpp"
#include "glm/glm.hpp"

namespace GLCore
{

class EngineEvent : public Event
{
protected:
    EngineEvent()
    {
    }
};

class ColliderLocationChangedEvent : public EngineEvent
{
public:
    ColliderLocationChangedEvent(glm::vec3 location) : m_Location(location)
    {
    }

    inline glm::vec3 GetLocation() const
    {
        return m_Location;
    }

    EVENT_CLASS_TYPE(ColliderLocationChanged)
    EVENT_CLASS_CATEGORY(EventCategoryEngine)
private:
    glm::vec3 m_Location;
};

}