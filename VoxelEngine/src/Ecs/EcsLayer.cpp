#include "EcsLayer.hpp"

namespace VoxelEngine
{

EcsLayer::EcsLayer(EngineState& state) : m_State(state)
{
}

EcsLayer::~EcsLayer()
{
}

void EcsLayer::OnAttach()
{
}

void EcsLayer::OnDetach()
{
}

void EcsLayer::OnUpdate(GLCore::Timestep ts)
{
}

}
