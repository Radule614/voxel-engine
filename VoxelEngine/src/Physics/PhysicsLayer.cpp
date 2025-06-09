#include "PhysicsLayer.hpp"
#include "PhysicsEngine.hpp"

namespace VoxelEngine
{

PhysicsLayer::PhysicsLayer(EngineState& state) : m_State(state)
{
}

void PhysicsLayer::OnAttach()
{
}

void PhysicsLayer::OnUpdate(const GLCore::Timestep ts)
{
    PhysicsEngine::Instance().OnUpdate(ts);
}

}
