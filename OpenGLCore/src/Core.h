#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <utility>

#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Log.h"

#ifdef GLCORE_ENABLE_ASSERTS
#define GLCORE_ASSERT                                                                                                  \
    (x, ...)                                                                                                           \
    {
if (!(x))
{
    LOG_ERROR("Assertion Failed: {0}", __VA_ARGS__)
}
}
#else
#define GLCORE_ASSERT (x, ...)
#endif
