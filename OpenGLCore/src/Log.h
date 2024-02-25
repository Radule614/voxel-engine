#pragma once

#include "Core.h"
#include <spdlog/spdlog.h>

namespace GLCore
{
class Log
{
    public:
    static void Init();
    inline static std::shared_ptr<spdlog::logger> s_Logger;
    // inline static std::shared_ptr<int> s_Logger;
};
} // namespace GLCore
