#pragma once

#include "Event.h"

namespace GLCore
{
class WindowCloseEvent : public Event
{
public:
    WindowCloseEvent()
    {
    }

    EVENT_CLASS_TYPE(WindowClose)
    EVENT_CLASS_CATEGORY(EventCategoryApplication)
};
} // namespace GLCore
