#include "Core/ECS/Systems/Physics/NarrowPhase.h"
#include "Core/ECS/ECSManager.h"
#include "Components/BoxCollider2D.h"
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

	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::BoxCollider2D>(
		[&](const Components::Transform* transform, Components::BoxCollider2D *box)
		{
			box->UpdatePositionAndRotation(transform->Position, transform->Rotation);
			box->IsColliding = false;
		});

	SolveCircleVsCircle();
	SolveBoxVsBox();
	// SolveAABBVsAABB();
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

			if (CollisionChecks::CircleVsCircle(firstTransform->Position,
			                                    secondTransform->Position,
			                                    radiusSum))
			{
				firstCircle->IsColliding = true;
				secondCircle->IsColliding = true;

				//Resolve Circle Vs Circle
				glm::vec2 directionVector = secondTransform->Position - firstTransform->Position;
				if (glm::length(directionVector) > 0.0f)
				{
					glm::vec2 normal = glm::normalize(directionVector);
					float penetrationDepth = radiusSum - glm::distance(firstTransform->Position, secondTransform->Position);
					firstTransform->Position += -penetrationDepth * 0.5f * normal;
					secondTransform->Position += penetrationDepth * 0.5f * normal;
				}
			}
		}
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::SolveAABBVsAABB()
{
	auto [iterator1, view] = ECSManager::GetInstance().GetView<Components::Transform, Components::BoxCollider2D>();

	for (iterator1; iterator1 != view.end(); ++iterator1)
	{
		auto firstTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*iterator1);
		auto firstAABB = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*iterator1);

		for (auto iterator2 = iterator1+1; iterator2 != view.end(); ++iterator2)
		{
			auto secondTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*iterator2);
			auto secondAABB = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*iterator2);

			if (CollisionChecks::AABBVsAABB(firstTransform->Position,
			                                secondTransform->Position,
			                                firstAABB->GetHalfExtents(),
			                                secondAABB->GetHalfExtents()))
			{
				firstAABB->IsColliding = true;
				secondAABB->IsColliding = true;
			}
		}
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::SolveBoxVsBox()
{
	static std::vector<const glm::vec2*> firstBoxPoints = std::vector<const glm::vec2*>(4);
	static std::vector<const glm::vec2*> secondBoxPoints = std::vector<const glm::vec2*>(4);

	auto [entityID1, view] = ECSManager::GetInstance().GetView<Components::BoxCollider2D>();

	for (entityID1; entityID1 != view.end(); ++entityID1)
	{
		auto firstBox = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*entityID1);

		firstBoxPoints = {&firstBox->GetTopLeftPoint(),
			&firstBox->GetTopRightPoint(),
		    &firstBox->GetBottomRightPoint(),
			&firstBox->GetBottomLeftPoint()};

		for (auto entityID2 = entityID1+1; entityID2 != view.end(); ++entityID2)
		{
			auto secondBox = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*entityID2);

			secondBoxPoints = {&secondBox->GetTopLeftPoint(),
				&secondBox->GetTopRightPoint(),
				&secondBox->GetBottomRightPoint(),
				&secondBox->GetBottomLeftPoint()};

			if (CollisionChecks::PolygonVsPolygon(firstBoxPoints, secondBoxPoints))
			{
				firstBox->IsColliding = true;
				secondBox->IsColliding = true;
			}
		}
	}
}



