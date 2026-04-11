#include "Core/ECS/Systems/Physics/NarrowPhase.h"
#include "Core/ECS/ECSManager.h"
#include "Components/PolygonCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/Systems/Physics/CollisionChecks.h"
#include "Components/Rigidbody2D.h"
#include "Core/ECS/Systems/Physics/CollisionPair.h"
#include "Core/ECS/Systems/Physics/CollisionManifold.h"


void Core::ECS::Systems::Physics::NarrowPhase::GenerateManifolds(const std::vector<CollisionPair>& collisionPairs, std::vector<CollisionManifold>& manifolds)
{
	for (auto& pair : collisionPairs)
	{
		collisionFunctions[pair.Type](pair, manifolds);
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::DoCircleVsCircle(const CollisionPair& pair, std::vector<CollisionManifold>& manifolds)
{
	auto firstCircle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(pair.EntityA);
	auto secondCircle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(pair.EntityB);

	auto firstCircleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto secondCircleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);

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
				ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
				ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
				std::forward<glm::vec2>(contactNormal),
				penetrationDepth);
		}
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::DoPolygonVsCircle(const CollisionPair& pair, std::vector<CollisionManifold>& manifolds)
{
	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);

	auto polygon = ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityA);
	auto circle = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(pair.EntityB);

	auto polygonTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto circleTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);

	if (CollisionChecks::PolygonVsCircle(polygonTransform->Position,
				polygon->GetPoints(), circleTransform->Position,
				circle->Radius, penetrationDepth, contactNormal))
	{
		polygon->IsColliding = true;
		circle->IsColliding = true;

		manifolds.emplace_back(polygonTransform,
			circleTransform,
			ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
			ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
			std::forward<glm::vec2>(contactNormal),
			penetrationDepth);
	}
}

void Core::ECS::Systems::Physics::NarrowPhase::DoPolygonVsPolygon(const CollisionPair& pair, std::vector<CollisionManifold>& manifolds)
{
	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);

	auto firstPolygon = ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityA);
	auto secondPolygon = ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityB);

	auto firstPolygonTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto secondPolygonTransform = ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);


	if (CollisionChecks::PolygonVsPolygon(firstPolygon->GetCenter(),
				secondPolygon->GetCenter(),
				firstPolygon->GetPoints(),
				secondPolygon->GetPoints(),
				penetrationDepth,
				contactNormal))
	{
		firstPolygon->IsColliding = true;
		secondPolygon->IsColliding = true;

		manifolds.emplace_back(firstPolygonTransform,
			secondPolygonTransform,
			ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
			ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
			std::forward<glm::vec2>(contactNormal),
			penetrationDepth);
	}
}

