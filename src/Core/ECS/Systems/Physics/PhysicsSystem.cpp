#include "Core/ECS/Systems/Physics/PhysicsSystem.h"
#include "Components/CircleCollider2D.h"
#include "Components/PolygonCollider2D.h"
#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/Physics/NarrowPhase.h"

Core::ECS::Systems::PhysicsSystem* Core::ECS::Systems::PhysicsSystem::m_instance = nullptr;

Core::ECS::Systems::PhysicsSystem *Core::ECS::Systems::PhysicsSystem::GetInstance()
{
	return m_instance;
}

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
	ECSManager::GetInstance().RegisterInterestedComponentsForSystem
			<Components::Transform, Components::Rigidbody2D>(this);

#ifdef _DEBUG
	m_shouldRunOnlyWhilePlaying = true;
#endif
}

void Core::ECS::Systems::PhysicsSystem::BeginSystem()
{
	m_broadPhase = new Physics::NaiveBroadPhase();
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
	accumulator += deltaTime;

	while (accumulator >= m_timeStep)
	{
		UpdateRigidbodies(m_timeStep);
		UpdateColliders();
		m_broadPhase->GeneratePairs(m_collisionPairs);
		m_narrowPhase.GenerateManifolds(m_collisionPairs, m_collisionManifolds);
		m_solver.Solve(m_collisionManifolds);
		m_collisionManifolds.clear();
		accumulator -= m_timeStep;
	}
}

void Core::ECS::Systems::PhysicsSystem::UpdateRigidbodies(const float physicsTimeStep)
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::Rigidbody2D>(
		[&](Components::Transform *transform, Components::Rigidbody2D *rigidbody)
		{
			rigidbody->LinearVelocity += rigidbody->Acceleration * physicsTimeStep;
			// rigidbody->LinearVelocity += m_gravity * physicsTimeStep;
			transform->Position += rigidbody->LinearVelocity * physicsTimeStep;
		});
}

void Core::ECS::Systems::PhysicsSystem::UpdateColliders()
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
		[&](const Components::Transform *transform, Components::CircleCollider2D *circleCollider)
		{
			circleCollider->UpdatePosition(transform->Position);
			circleCollider->IsColliding = false;
		});

	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::PolygonCollider2D>(
		[&](const Components::Transform *transform, Components::PolygonCollider2D *polygon)
		{
			polygon->UpdatePositionAndRotation(transform->Position, transform->Rotation);
			polygon->IsColliding = false;
		});
}
