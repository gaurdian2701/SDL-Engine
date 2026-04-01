#include "Core/ECS/Systems/Physics/PhysicsSystem.h"
#include "Components/BoxCollider2D.h"
#include "Components/Rigidbody2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/Physics/NarrowPhase.h"

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem
        <Components::Transform, Components::Rigidbody2D>(this);

#ifdef _DEBUG
	m_shouldRunOnlyWhilePlaying = true;
#endif
}

void Core::ECS::Systems::PhysicsSystem::ProcessInitializationQueue()
{
	for (auto entity : m_initializationQueue)
	{
		auto rigidbody = ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(entity);
		rigidbody->LinearVelocity += m_gravity;
	}
}

void Core::ECS::Systems::PhysicsSystem::BeginSystem()
{
	//TODO: Create Broad Phase
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
	System::UpdateSystem(deltaTime);

	accumulator += deltaTime;

	while (accumulator >= m_timeStep)
	{
		UpdateRigidbodies(m_timeStep);
		m_narrowPhase.GenerateManifolds(m_collisionManifolds);
		m_solver.Solve(m_collisionManifolds);
		m_collisionManifolds.clear();
		accumulator -= m_timeStep;
	}
}

void Core::ECS::Systems::PhysicsSystem::UpdateRigidbodies(const float physicsTimeStep)
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::Rigidbody2D>(
		[&](Components::Transform* transform, Components::Rigidbody2D* rigidbody)
		{
			rigidbody->LinearVelocity += m_gravity * physicsTimeStep;
			transform->Position += rigidbody->LinearVelocity * physicsTimeStep;
		});
}
