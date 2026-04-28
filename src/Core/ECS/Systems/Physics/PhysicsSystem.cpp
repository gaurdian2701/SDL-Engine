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

void Core::ECS::Systems::PhysicsSystem::OnComponentAdded(const std::uint32_t entityID)
{
	auto transform = ECSManager::GetInstance().GetComponent<Components::Transform>(entityID);
	auto rigidbody = ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(entityID);

	if (auto circle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(entityID))
	{
		circle->SetPosition(transform->Position);
		circle->IsColliding = false;

		if (circle->MatchScaleWithTransform)
		{
			circle->SetRadius(glm::length(transform->Scale) * 0.5f);
		}

		rigidbody->SetMomentOfInertia(circle->CalculateMomentOfInertia(rigidbody->GetMass()));
	}
	else if (auto polygon = ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(entityID))
	{
		polygon->UpdatePositionAndRotation(transform->Position, transform->Rotation);
		polygon->IsColliding = false;

		if (polygon->MatchScaleWithTransform)
		{
			polygon->SetBoxHalfExtents(transform->Scale * 0.5f);
		}

		rigidbody->SetMomentOfInertia(polygon->CalculateMomentOfInertia(rigidbody->GetMass()));
	}
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
		//Update positions and colliders before physics step
		IntegrateVelocities(m_timeStep);
		IntegratePositions(m_timeStep);
		UpdateCollidersAndRigidbodies();

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
		transform->Rotation += rigidbody->AngularVelocity * physicsTimeStep;
		rigidbody->LinearAcceleration = glm::vec2(0.0f);
	});
}

void Core::ECS::Systems::PhysicsSystem::UpdateCollidersAndRigidbodies()
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D, Components::Rigidbody2D>(
		[&](const Components::Transform *transform, Components::CircleCollider2D *circleCollider,
			Components::Rigidbody2D* rigidbody)
		{
			circleCollider->SetPosition(transform->Position);
			circleCollider->IsColliding = false;
			if (rigidbody->MassDirtyByte)
			{
				rigidbody->SetMomentOfInertia(circleCollider->CalculateMomentOfInertia(rigidbody->GetMass()));
				rigidbody->MassDirtyByte = false;
			}
		});

	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::PolygonCollider2D, Components::Rigidbody2D>(
		[&](const Components::Transform *transform, Components::PolygonCollider2D *polygon,
			Components::Rigidbody2D* rigidbody)
		{
			polygon->UpdatePositionAndRotation(transform->Position, transform->Rotation);
			polygon->IsColliding = false;
			if (rigidbody->MassDirtyByte)
			{
				rigidbody->SetMomentOfInertia(polygon->CalculateMomentOfInertia(rigidbody->GetMass()));
				rigidbody->MassDirtyByte = false;
			}
		});
}
