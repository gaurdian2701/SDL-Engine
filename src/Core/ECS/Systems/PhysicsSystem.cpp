#include "Core/ECS/Systems/PhysicsSystem.h"
#include "Components/AABB2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
	ECSManager::GetInstance().RegisterInterestedComponentsForSystem
			<Components::Transform, Components::AABB2D>(this);
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{

}
