#include "Core/Physics/NarrowPhase.h"
#include "Core/ECS/ECSManager.h"
#include "Components/PolygonCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Components/Rigidbody2D.h"
#include "Core/ECS/Systems/DebugDrawSystem.h"
#include "Core/Physics/ShapeOverlapFunctions.h"
#include "Core/Physics/ContactManifold.h"
#include "Core/Physics/CollisionPair.h"
#include "Core/Physics/ContactPointFunctions.h"

void Core::Physics::NarrowPhase::GenerateManifolds(
	const std::vector<Core::Physics::PhysicsData::CollisionPair>& collisionPairs,
	std::vector<PhysicsData::ContactManifold>& manifolds)
{
	for (auto& pair : collisionPairs)
	{
		collisionFunctions[pair.Type](pair, manifolds);
	}
}

void Core::Physics::NarrowPhase::DoCircleVsCircle(const Core::Physics::PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds)
{
	auto circleA = Core::ECS::ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(pair.EntityA);
	auto circleB = Core::ECS::ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(pair.EntityB);

	auto firstCircleTransform = Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto secondCircleTransform = Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);

	float radiusSum = circleA->GetRadius() + circleB->GetRadius();

	if (ShapeOverlapFunctions::CircleVsCircle(firstCircleTransform->Position,
									secondCircleTransform->Position,
									radiusSum))
	{
		circleA->IsColliding = true;
		circleB->IsColliding = true;

		glm::vec2 contactPoint = glm::vec2(0.0f);

		FindCircleVsCircleContactPoint(*circleA, *circleB, contactPoint);

		DoDebugCode(
			ECS::Systems::DebugDrawSystem* debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
			debugSystem->DrawHollowCircle(contactPoint, 10.0f, 255, 240, 0, 255);
			);

		//Resolve Circle Vs Circle
		glm::vec2 directionVector = secondCircleTransform->Position - firstCircleTransform->Position;
		if (glm::length(directionVector) > 0.0f)
		{
			glm::vec2 contactNormal = glm::normalize(directionVector);
			float penetrationDepth = radiusSum - glm::distance(firstCircleTransform->Position, secondCircleTransform->Position);

			manifolds.emplace_back(firstCircleTransform, secondCircleTransform,
				Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
				Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
				std::forward<glm::vec2>(contactNormal),
				penetrationDepth);
		}
	}
}

void Core::Physics::NarrowPhase::DoPolygonVsCircle(const Core::Physics::PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds)
{
	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);

	auto polygon = Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityA);
	auto circle = Core::ECS::ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(pair.EntityB);

	auto polygonTransform = Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto circleTransform = Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);

	std::uint32_t closestPolygonVertexIndex = 0;

	if (Core::Physics::ShapeOverlapFunctions::PolygonVsCircle(polygonTransform->Position,
				polygon->GetPoints(),
				circleTransform->Position,
				circle->GetRadius(),
				penetrationDepth,
				contactNormal,
				closestPolygonVertexIndex))
	{
		polygon->IsColliding = true;
		circle->IsColliding = true;

		glm::vec2 contactPoint = glm::vec2(0.0f);

		FindPolygonVsCircleContactPoint(*polygon,
			*circle,
			contactPoint,
			closestPolygonVertexIndex);

		DoDebugCode(
		ECS::Systems::DebugDrawSystem* debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
		debugSystem->DrawHollowCircle(contactPoint, 10.0f, 255, 240, 0, 255);
		);

		manifolds.emplace_back(polygonTransform,
			circleTransform,
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
			std::forward<glm::vec2>(contactNormal),
			penetrationDepth);
	}
}

void Core::Physics::NarrowPhase::DoPolygonVsPolygon(const Core::Physics::PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds)
{
	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);
	glm::vec2 referenceEdge = glm::vec2(0.0f);
	bool aHasReferenceEdge = false;
	std::size_t referenceEdgeIndex = 0;

	auto firstPolygon =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityA);
	auto secondPolygon =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityB);

	auto firstPolygonTransform =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto secondPolygonTransform =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);

	if (Core::Physics::ShapeOverlapFunctions::PolygonVsPolygon(firstPolygon->GetCenter(),
				secondPolygon->GetCenter(),
				firstPolygon->GetPoints(),
				secondPolygon->GetPoints(),
				penetrationDepth,
				contactNormal,
				referenceEdge,
				aHasReferenceEdge,
				referenceEdgeIndex))
	{
		firstPolygon->IsColliding = true;
		secondPolygon->IsColliding = true;

		static std::vector<glm::vec2> contactPoints = std::vector<glm::vec2>(2);
		contactPoints.clear();

		FindPolygonVsPolygonContactPoints(*firstPolygon,
			*secondPolygon,
			aHasReferenceEdge,
			referenceEdge,
			contactNormal,
			referenceEdgeIndex,
			contactPoints);

		DoDebugCode(
		ECS::Systems::DebugDrawSystem* debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
		if (!contactPoints.empty())
		{
			debugSystem->DrawHollowCircle(contactPoints[0], 10.0f, 255, 240, 0, 255);

			if (contactPoints.size() > 1)
			{
				debugSystem->DrawHollowCircle(contactPoints[1], 10.0f, 255, 240, 0, 255);
			}
		}
		);

		manifolds.emplace_back(firstPolygonTransform,
			secondPolygonTransform,
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
			std::forward<glm::vec2>(contactNormal),
			penetrationDepth);
	}
}

