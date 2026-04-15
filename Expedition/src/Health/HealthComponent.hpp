#pragma once

#include <algorithm>

namespace Expedition
{

struct HealthComponent
{
    float MaxHealth     = 100.0f;
    float CurrentHealth = 100.0f;

    bool IsDead()                const { return CurrentHealth <= 0.0f; }
    void TakeDamage(float amount)      { CurrentHealth = std::max(0.0f, CurrentHealth - amount); }
};

}
