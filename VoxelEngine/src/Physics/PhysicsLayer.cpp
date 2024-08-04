#include "PhysicsLayer.hpp"
#include "PhysicsEngine.hpp"

namespace VoxelEngine
{

PhysicsLayer::PhysicsLayer(EngineState& state) : m_State(state)
{
}

PhysicsLayer::~PhysicsLayer()
{
}

void PhysicsLayer::OnAttach()
{
}

void PhysicsLayer::OnUpdate(GLCore::Timestep ts)
{
	PhysicsEngine::Instance().OnUpdate(ts);
}

}
