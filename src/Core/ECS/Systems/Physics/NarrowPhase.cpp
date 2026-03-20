#include "Core/ECS/Systems/Physics/NarrowPhase.h"
#include "Core/ECS/ECSManager.h"
#include "Components/AABB2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/Systems/Physics/CollisionChecks.h"

void Core::ECS::Systems::Physics::NarrowPhase::DoNarrowPhase(const float deltaTime)
{
	ECSManager::GetInstance().ForEachUsingComponents<Components::CircleCollider2D>(
	[&](Components::CircleCollider2D *circleCollider)
	{
		circleCollider->IsColliding = false;
	});

	ECSManager::GetInstance().ForEachUsingComponents<Components::AABB2D>(
		[&](Components::AABB2D *box)
		{
			box->IsColliding = false;
		});

	SolveCircleVsCircle();
	SolveAABBVsAABB();
}

void Core::ECS::Systems::Physics::NarrowPhase::SolveCircleVsCircle()
{
	auto [iterator1, view] = ECSManager::GetInstance().GetView<Components::Transform, Components::CircleCollider2D>();

	for (iterator1; iterator1 != view.end(); ++iterator1)
	{
		auto firstTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*iterator1);
		auto firstCircle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*iterator1);

		for (auto iterator2 = iterator1+1; iterator2 != view.end(); ++iterator2)
		{
			auto secondTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*iterator2);
			auto secondCircle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*iterator2);
			float radiusSum = firstCircle->Radius + secondCircle->Radius;

			if (CirclevsCircle(firstTransform->WorldPosition,
			secondTransform->WorldPosition,
			radiusSum))
			{
				firstCircle->IsColliding = true;
				secondCircle->IsColliding = true;

				//Resolve Circle Vs Circle
				glm::vec2 directionVector = secondTransform->WorldPosition - firstTransform->WorldPosition;
				if (glm::length(directionVector) > 0.0f)
				{
					glm::vec2 normal = glm::normalize(directionVector);
					float penetrationDepth = radiusSum - glm::distance(firstTransform->WorldPosition, secondTransform->WorldPosition);
					firstTransform->LocalPosition += -penetrationDepth * 0.5f * normal;
					secondTransform->LocalPosition += penetrationDepth * 0.5f * normal;
				}
			}
		}
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::SolveAABBVsAABB()
{
	auto [iterator1, view] = ECSManager::GetInstance().GetView<Components::Transform, Components::AABB2D>();

	for (iterator1; iterator1 != view.end(); ++iterator1)
	{
		auto firstTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*iterator1);
		auto firstAABB = ECSManager::GetInstance().GetComponent<Components::AABB2D>(*iterator1);

		for (auto iterator2 = iterator1+1; iterator2 != view.end(); ++iterator2)
		{
			auto secondTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*iterator2);
			auto secondAABB = ECSManager::GetInstance().GetComponent<Components::AABB2D>(*iterator2);

			if (AABBvsAABB(firstTransform->WorldPosition,
			secondTransform->WorldPosition,
			firstAABB->HalfExtents,
			secondAABB->HalfExtents))
			{
				firstAABB->IsColliding = true;
				secondAABB->IsColliding = true;
			}
		}
	}
}


