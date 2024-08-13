#pragma once

#include "Event.hpp"

namespace GLCore
{

class StateEvent : public Event
{
protected: 
    StateEvent()
    {
    }
};

class StatePauseEvent : public StateEvent
{
public:
    StatePauseEvent()
    {
    }

    EVENT_CLASS_TYPE(StatePause)
    EVENT_CLASS_CATEGORY(EventCategoryState)
};

class StateUnpauseEvent : public StateEvent
{
public:
    StateUnpauseEvent()
    {
    }

    EVENT_CLASS_TYPE(StateUnpause)
    EVENT_CLASS_CATEGORY(EventCategoryState)
};

} 
