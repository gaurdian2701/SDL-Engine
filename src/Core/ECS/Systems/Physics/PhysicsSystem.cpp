#include "Core/ECS/Systems/Physics/PhysicsSystem.h"
#include "Components/BoxCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/Physics/NarrowPhase.h"

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem
        <Components::Transform, Components::BoxCollider2D>(this);
}

void Core::ECS::Systems::PhysicsSystem::BeginSystem()
{
	//TODO: Create Broad Phase
	m_narrowPhase = new Physics::NarrowPhase();
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
	m_narrowPhase->DoNarrowPhase(deltaTime);
}