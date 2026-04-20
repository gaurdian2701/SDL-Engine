#include "../../../../../include/Core/ECS/Systems/PhysicsSystem.h"
#include "Components/CircleCollider2D.h"
#include "Components/PolygonCollider2D.h"
#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "../../../../../include/Core/Physics/NarrowPhase.h"

Core::ECS::Systems::PhysicsSystem* Core::ECS::Systems::PhysicsSystem::m_instance = nullptr;

Core::ECS::Systems::PhysicsSystem *Core::ECS::Systems::PhysicsSystem::GetInstance()
{
	return m_instance;
}

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
	ECSManager::GetInstance().RegisterInterestedComponentsForSystem
			<Components::Transform, Components::Rigidbody2D>(this);

	DoDebugStatement(m_shouldRunOnlyWhilePlaying = true);
}

void Core::ECS::Systems::PhysicsSystem::BeginSystem()
{
	m_broadPhase = new Physics::NaiveBroadPhase();

	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
	[&](const Components::Transform *transform, Components::CircleCollider2D *circleCollider)
	{
		circleCollider->SetPosition(transform->Position);
		circleCollider->IsColliding = false;

		if (circleCollider->MatchScaleWithTransform)
		{
			circleCollider->SetRadius(glm::length(transform->Scale) * 0.5f);
		}
	});

	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::PolygonCollider2D>(
		[&](const Components::Transform *transform, Components::PolygonCollider2D *polygon)
		{
			polygon->UpdatePositionAndRotation(transform->Position, transform->Rotation);
			polygon->IsColliding = false;

			if (polygon->MatchScaleWithTransform)
			{
				polygon->SetBoxHalfExtents(transform->Scale * 0.5f);
			}
		});
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
	accumulator += deltaTime;

	while (accumulator >= m_timeStep)
	{
		IntegrateVelocities(m_timeStep);
		IntegratePositions(m_timeStep);
		UpdateColliders();

		m_broadPhase->GeneratePairs(m_collisionPairs);
		m_narrowPhase.GenerateManifolds(m_collisionPairs, m_collisionManifolds);
		m_solver.Solve(m_collisionManifolds, m_timeStep);
		m_collisionManifolds.clear();
		accumulator -= m_timeStep;
	}
}

void Core::ECS::Systems::PhysicsSystem::IntegrateVelocities(const float physicsTimeStep)
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Rigidbody2D>(
	[&](Components::Rigidbody2D *rigidbody)
	{
		rigidbody->AddForce(m_gravity * rigidbody->GetMass());
		rigidbody->LinearVelocity += rigidbody->LinearAcceleration * physicsTimeStep;
	});
}

void Core::ECS::Systems::PhysicsSystem::IntegratePositions(const float physicsTimeStep)
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::Rigidbody2D>(
	[&](Components::Transform *transform, Components::Rigidbody2D *rigidbody)
	{
		transform->Position += rigidbody->LinearVelocity * physicsTimeStep;
		rigidbody->LinearAcceleration = glm::vec2(0.0f);
	});
}

void Core::ECS::Systems::PhysicsSystem::UpdateColliders()
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
		[&](const Components::Transform *transform, Components::CircleCollider2D *circleCollider)
		{
			circleCollider->SetPosition(transform->Position);
			circleCollider->IsColliding = false;
		});

	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::PolygonCollider2D>(
		[&](const Components::Transform *transform, Components::PolygonCollider2D *polygon)
		{
			polygon->UpdatePositionAndRotation(transform->Position, transform->Rotation);
			polygon->IsColliding = false;
		});
}
