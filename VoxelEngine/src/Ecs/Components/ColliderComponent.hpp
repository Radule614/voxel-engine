#pragma once

#include "../../Physics/PhysicsEngine.hpp"

namespace VoxelEngine
{

class ColliderComponent
{
public:
	ColliderComponent(JPH::BodyID bodyId, JPH::Shape* shape);
	~ColliderComponent();

	JPH::BodyID GetBodyId() const;

private:
	JPH::BodyID m_BodyId;

	//TODO: Figure out how and where to store this instead of using the raw pointer. (memory leak)
	JPH::Shape* m_Shape;
};

class ColliderFactory
{
public:
	static ColliderComponent CreateSphereCollider(float_t r, glm::vec3 p);
	static ColliderComponent CreateBoxCollider();
};

}