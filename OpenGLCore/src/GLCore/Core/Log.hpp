#pragma once

#include "Core.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

namespace GLCore
{
class Log
{
public:
    static void Init();

    inline static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }

private:
    static std::shared_ptr<spdlog::logger> s_Logger;
};
}

#define LOG_TRACE(...) ::GLCore::Log::GetLogger()->log(spdlog::level::trace, std::format(__VA_ARGS__));
#define LOG_INFO(...) ::GLCore::Log::GetLogger()->log(spdlog::level::info, std::format(__VA_ARGS__));
#define LOG_WARN(...) ::GLCore::Log::GetLogger()->log(spdlog::level::warn, std::format(__VA_ARGS__));
#define LOG_ERROR(...) ::GLCore::Log::GetLogger()->log(spdlog::level::err, std::format(__VA_ARGS__));
#define LOG_CRITICAL(...) ::GLCore::Log::GetLogger()->log(spdlog::level::critical, std::format(__VA_ARGS__));
