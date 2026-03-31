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

}

void Core::ECS::Systems::PhysicsSystem::BeginSystem()
{
	//TODO: Create Broad Phase
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
	System::UpdateSystem(deltaTime);
	UpdateRigidbodies(deltaTime);
	m_narrowPhase.GenerateManifolds(m_collisionManifolds);
	m_solver.Solve(m_collisionManifolds);
	m_collisionManifolds.clear();
}

void Core::ECS::Systems::PhysicsSystem::UpdateRigidbodies(const float deltaTime)
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::Rigidbody2D>(
		[&](Components::Transform* transform, Components::Rigidbody2D* rigidbody)
		{
			rigidbody->Velocity += m_gravity * deltaTime;
			transform->Position += rigidbody->Velocity * deltaTime;
		});
}
