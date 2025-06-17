//
// Created by RadU on 15-Jun-25.
//

#include "PlayerCharacterManager.hpp"

using namespace JPH;

namespace VoxelEngine
{

class PlayerBroadPhaseLayerFilter final : public BroadPhaseLayerFilter
{
public:
    bool ShouldCollide(const BroadPhaseLayer inLayer) const override
    {
        if (inLayer == BroadPhaseLayers::NON_MOVING) { return true; }
        if (inLayer == BroadPhaseLayers::MOVING) { return true; }
        JPH_ASSERT(false);
        return false;
    }
};

class PlayerObjectLayerFilter final : public ObjectLayerFilter
{
public:
    bool ShouldCollide(const ObjectLayer inLayer) const override
    {
        switch (inLayer)
        {
            case Layers::NON_MOVING:
            case Layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
        }
    }
};

class PlayerBodyFilter final : public BodyFilter
{
};

class PlayerShapeFilter final : public ShapeFilter
{
};

PlayerCharacterManager::PlayerCharacterManager()
    : m_TempAllocator(std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024)),
      m_BroadPhaseLayerFilter(std::make_unique<PlayerBroadPhaseLayerFilter>()),
      m_ObjectLayerFilter(std::make_unique<PlayerObjectLayerFilter>()),
      m_BodyFilter(std::make_unique<PlayerBodyFilter>()),
      m_ShapeFilter(std::make_unique<PlayerShapeFilter>())
{
}

PlayerCharacterManager::~PlayerCharacterManager() = default;

void PlayerCharacterManager::UpdateCharacterVirtual(CharacterVirtual& character,
                                                    const float_t deltaTime,
                                                    const Vec3 inGravity) const
{
    character.Update(deltaTime,
                     inGravity,
                     *m_BroadPhaseLayerFilter,
                     *m_ObjectLayerFilter,
                     *m_BodyFilter,
                     *m_ShapeFilter,
                     *m_TempAllocator);
}

}
