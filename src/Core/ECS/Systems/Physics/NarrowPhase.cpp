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
	SolveBoxVsCircle();
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
					glm::vec2 contactNormal = glm::normalize(directionVector);
					float penetrationDepth = radiusSum - glm::distance(firstTransform->Position, secondTransform->Position);
					if (!firstTransform->Static)
					{
						firstTransform->Position += -penetrationDepth * 0.5f * contactNormal;
					}

					if (!secondTransform->Static)
					{
						secondTransform->Position += penetrationDepth * 0.5f * contactNormal;
					}
				}
			}
		}
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::SolveBoxVsBox()
{
	static std::vector<const glm::vec2*> firstBoxPoints = std::vector<const glm::vec2*>(4);
	static std::vector<const glm::vec2*> secondBoxPoints = std::vector<const glm::vec2*>(4);

	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);

	auto [entityID1, view] = ECSManager::GetInstance().GetView<Components::Transform,
	Components::BoxCollider2D>();

	for (entityID1; entityID1 != view.end(); ++entityID1)
	{
		auto firstTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*entityID1);
		auto firstBox = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*entityID1);

		firstBoxPoints = {&firstBox->GetTopLeftPoint(),
			&firstBox->GetTopRightPoint(),
		    &firstBox->GetBottomRightPoint(),
			&firstBox->GetBottomLeftPoint()};

		for (auto entityID2 = entityID1+1; entityID2 != view.end(); ++entityID2)
		{
			auto secondTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*entityID2);
			auto secondBox = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*entityID2);

			secondBoxPoints = {&secondBox->GetTopLeftPoint(),
				&secondBox->GetTopRightPoint(),
				&secondBox->GetBottomRightPoint(),
				&secondBox->GetBottomLeftPoint()};

			if (CollisionChecks::PolygonVsPolygon(firstBox->GetCenter(),
				secondBox->GetCenter(),
				firstBoxPoints,
				secondBoxPoints,
				penetrationDepth,
				contactNormal))
			{
				firstBox->IsColliding = true;
				secondBox->IsColliding = true;

				if (!firstTransform->Static)
				{
					firstTransform->Position += -penetrationDepth * 0.5f * contactNormal;
				}

				if (!secondTransform->Static)
				{
					secondTransform->Position += penetrationDepth * 0.5f * contactNormal;
				}
			}
		}
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::SolveBoxVsCircle()
{
	static std::vector<const glm::vec2*> boxPoints = std::vector<const glm::vec2*>(4);

	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);

	auto [entityID1, boxView] = ECSManager::GetInstance().GetView<Components::Transform, Components::BoxCollider2D>();
	auto [entityID2, circleView] = ECSManager::GetInstance().GetView<Components::Transform, Components::CircleCollider2D>();

	for (entityID1; entityID1 != boxView.end(); ++entityID1)
	{
		auto boxTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*entityID1);
		auto boxCollider = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*entityID1);

		boxPoints = {&boxCollider->GetTopLeftPoint(),
			&boxCollider->GetTopRightPoint(),
			&boxCollider->GetBottomRightPoint(),
			&boxCollider->GetBottomLeftPoint()};

		for (entityID2 = circleView.begin() + 1; entityID2 != circleView.end(); ++entityID2)
		{
			auto circleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*entityID2);
			auto circleCollider = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*entityID2);

			if (CollisionChecks::PolygonVsCircle(boxTransform->Position,
				boxPoints, circleTransform->Position,
				circleCollider->Radius, penetrationDepth, contactNormal))
			{
				boxCollider->IsColliding = true;
				circleCollider->IsColliding = true;

				if (!boxTransform->Static)
				{
					boxTransform->Position -= penetrationDepth * 0.5f * contactNormal;
				}
				if (!circleTransform->Static)
				{
					circleTransform->Position += penetrationDepth * 0.5f * contactNormal;
				}
			}
		}
	}
}



