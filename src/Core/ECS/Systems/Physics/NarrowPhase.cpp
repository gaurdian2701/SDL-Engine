#include "Core/ECS/Systems/Physics/NarrowPhase.h"
#include "Core/ECS/ECSManager.h"
#include "Components/BoxCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/Systems/Physics/CollisionChecks.h"
#include "Components/Rigidbody2D.h"

void Core::ECS::Systems::Physics::NarrowPhase::GenerateManifolds(std::vector<CollisionManifold>& manifolds)
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

	GenerateBoxCollisionManifolds(manifolds);
	GenerateCircleCollisionManifolds(manifolds);
}

void Core::ECS::Systems::Physics::NarrowPhase::GenerateBoxCollisionManifolds(std::vector<CollisionManifold>& manifolds)
{
	static std::vector<const glm::vec2*> firstBoxPoints = std::vector<const glm::vec2*>(4);
	static std::vector<const glm::vec2*> secondBoxPoints = std::vector<const glm::vec2*>(4);

	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);

	auto [boxEntityID1, boxView] = ECSManager::GetInstance().GetView<Components::Transform,
	Components::BoxCollider2D>();
	auto [circleEntityID, circleView] = ECSManager::GetInstance().GetView<Components::Transform,
	Components::CircleCollider2D>();

	for (boxEntityID1; boxEntityID1 != boxView.end(); ++boxEntityID1)
	{
		auto firstBoxTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*boxEntityID1);
		auto firstBox = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*boxEntityID1);

		firstBoxPoints = {&firstBox->GetTopLeftPoint(),
			&firstBox->GetTopRightPoint(),
			&firstBox->GetBottomRightPoint(),
			&firstBox->GetBottomLeftPoint()
		};

		//Solve Boxes vs Boxes first
		for (auto boxEntityID2 = boxEntityID1+1; boxEntityID2 != boxView.end(); ++boxEntityID2)
		{
			auto secondBoxTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*boxEntityID2);
			auto secondBox = ECSManager::GetInstance().GetComponent<Components::BoxCollider2D>(*boxEntityID2);

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

				manifolds.emplace_back(firstBoxTransform,
					secondBoxTransform,
					ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(*boxEntityID1),
					ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(*boxEntityID2),
					std::forward<glm::vec2>(contactNormal),
					penetrationDepth);
			}
		}

		//Solve Boxes vs Circles
		for (circleEntityID = circleView.begin() + 1; circleEntityID != circleView.end(); ++circleEntityID)
		{
			auto circleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*circleEntityID);
			auto circleCollider = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleEntityID);

			if (CollisionChecks::PolygonVsCircle(firstBoxTransform->Position,
				firstBoxPoints, circleTransform->Position,
				circleCollider->Radius, penetrationDepth, contactNormal))
			{
				firstBox->IsColliding = true;
				circleCollider->IsColliding = true;

				manifolds.emplace_back(firstBoxTransform,
					circleTransform,
					ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(*boxEntityID1),
					ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(*circleEntityID),
					std::forward<glm::vec2>(contactNormal),
					penetrationDepth);
			}
		}
	}
}


void Core::ECS::Systems::Physics::NarrowPhase::GenerateCircleCollisionManifolds(std::vector<CollisionManifold>& manifolds)
{
	auto [circleEntityID1, view] = ECSManager::GetInstance().GetView<Components::Transform, Components::CircleCollider2D>();

	for (circleEntityID1; circleEntityID1 != view.end(); ++circleEntityID1)
	{
		auto firstCircleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*circleEntityID1);
		auto firstCircle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleEntityID1);

		for (auto circleEntityID2 = circleEntityID1+1; circleEntityID2 != view.end(); ++circleEntityID2)
		{
			auto secondCircleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(*circleEntityID2);
			auto secondCircle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleEntityID2);
			float radiusSum = firstCircle->Radius + secondCircle->Radius;

			if (CollisionChecks::CircleVsCircle(firstCircleTransform->Position,
			                                    secondCircleTransform->Position,
			                                    radiusSum))
			{
				firstCircle->IsColliding = true;
				secondCircle->IsColliding = true;

				//Resolve Circle Vs Circle
				glm::vec2 directionVector = secondCircleTransform->Position - firstCircleTransform->Position;
				if (glm::length(directionVector) > 0.0f)
				{
					glm::vec2 contactNormal = glm::normalize(directionVector);
					float penetrationDepth = radiusSum - glm::distance(firstCircleTransform->Position, secondCircleTransform->Position);

					manifolds.emplace_back(firstCircleTransform, secondCircleTransform,
						ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(*circleEntityID1),
						ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(*circleEntityID2),
						std::forward<glm::vec2>(contactNormal),
						penetrationDepth);
				}
			}
		}
	}
}
