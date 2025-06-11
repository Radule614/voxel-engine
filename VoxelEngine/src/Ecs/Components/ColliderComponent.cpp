#include "ColliderComponent.hpp"

using namespace JPH;
using namespace JPH::literals;

namespace VoxelEngine
{

ColliderComponent::ColliderComponent(const JPH::BodyID bodyId) : m_BodyId(bodyId)
{
}

ColliderComponent::~ColliderComponent() = default;

BodyID ColliderComponent::GetBodyId() const
{
    return m_BodyId;
}

}
