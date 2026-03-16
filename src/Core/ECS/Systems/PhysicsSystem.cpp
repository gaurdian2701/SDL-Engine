#include "Core/ECS/Systems/PhysicsSystem.h"
#include "Assets/Components/Collider2D.h"
#include "Assets/Components/Transform.h"
#include "Core/ECS/ECSManager.h"

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
	ECSManager::GetInstance().RegisterInterestedComponentsForSystem
			<Assets::Components::Transform, Assets::Components::Collider2D>(this);
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
	ECSManager::GetInstance().ForEachUsingComponentsWithEntityID<Assets::Components::Transform, Assets::Components::Collider2D>(
		[&](const uint32_t firstEntity,
			const Assets::Components::Transform *firstTransform,
			Assets::Components::Collider2D *firstCollider)
		{
			glm::vec2 firstPosition = firstTransform->WorldPosition;
			firstCollider->EntityCollidedWith = INVALID_ENTITY_ID;

			ECSManager::GetInstance().BreakableForEachUsingComponentsWithEntityID<Assets::Components::Transform,
				Assets::Components::Collider2D>(
				[&](bool& breakOutOfLoop,
					const uint32_t secondEntity,
					const Assets::Components::Transform *secondTransform,
					Assets::Components::Collider2D *secondCollider)
			{
					if (firstEntity != secondEntity)
					{
						glm::vec2 secondPosition = secondTransform->WorldPosition;

						if (std::abs(firstPosition.x - secondPosition.x) < (firstCollider->HalfSize.x + secondCollider->HalfSize.x) &&
							std::abs(firstPosition.y - secondPosition.y) < (firstCollider->HalfSize.y + secondCollider->HalfSize.y))
						{
							firstCollider->EntityCollidedWith = secondEntity;
							secondCollider->EntityCollidedWith = firstEntity;
							breakOutOfLoop = true;
						}
					}
			});
		});
}
