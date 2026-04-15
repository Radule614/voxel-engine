#pragma once

namespace Expedition
{

struct SpoilsComponent
{
    int Amount = 0;
    void Add(const int n) { Amount += n; }
};

}
