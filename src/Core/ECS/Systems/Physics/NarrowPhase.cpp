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
#include "Core/Physics/ContactPoints.h"

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

		//Resolve Circle Vs Circle
		glm::vec2 directionVector = secondCircleTransform->Position - firstCircleTransform->Position;

		if (glm::length(directionVector) > 0.0f)
		{
			ContactPoints contactPoints = ContactPoints();
			contactPoints.NumberOfContactPoints = 1;
			FindCircleVsCircleContactPoint(*circleA, *circleB, contactPoints.Points[0]);

			DoDebug(
				ECS::Systems::DebugDrawSystem* debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
				debugSystem->DrawHollowCircle(contactPoints.Points[0], 10.0f, 255, 240, 0, 255);
				);

			glm::vec2 contactNormal = glm::normalize(directionVector);
			float penetrationDepth = radiusSum - glm::distance(firstCircleTransform->Position, secondCircleTransform->Position);

			manifolds.emplace_back(firstCircleTransform, secondCircleTransform,
				Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
				Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
				std::forward<glm::vec2>(contactNormal),
				penetrationDepth,
				contactPoints);
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
				polygon->GetVertices(),
				circleTransform->Position,
				circle->GetRadius(),
				penetrationDepth,
				contactNormal,
				closestPolygonVertexIndex))
	{
		polygon->IsColliding = true;
		circle->IsColliding = true;

		ContactPoints contactPoints = ContactPoints();
		contactPoints.NumberOfContactPoints = 1;

		FindPolygonVsCircleContactPoint(*polygon,
			*circle,
			contactPoints.Points[0],
			closestPolygonVertexIndex);

		DoDebug(
		ECS::Systems::DebugDrawSystem* debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
		debugSystem->DrawHollowCircle(contactPoints.Points[0], 10.0f, 255, 240, 0, 255);
		);

		manifolds.emplace_back(polygonTransform,
			circleTransform,
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
			std::forward<glm::vec2>(contactNormal),
			penetrationDepth,
			contactPoints);
	}
}

void Core::Physics::NarrowPhase::DoPolygonVsPolygon(const Core::Physics::PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds)
{
	float penetrationDepth = 0.0f;
	glm::vec2 contactNormal = glm::vec2(0.0f);
	glm::vec2 referenceEdge = glm::vec2(0.0f);
	bool aHasReferenceEdge = false;
	std::size_t referenceEdgeIndex = 0;

	auto polygonA =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityA);
	auto polygonB =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(pair.EntityB);

	auto firstPolygonTransform =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityA);
	auto secondPolygonTransform =
		Core::ECS::ECSManager::GetInstance().GetComponent<Components::Transform>(pair.EntityB);

	if (Core::Physics::ShapeOverlapFunctions::PolygonVsPolygon(polygonA->GetCenter(),
				polygonB->GetCenter(),
				polygonA->GetVertices(),
				polygonB->GetVertices(),
				penetrationDepth,
				contactNormal))
	{
		polygonA->IsColliding = true;
		polygonB->IsColliding = true;

		ShapeOverlapFunctions::FindReferenceEdge(polygonA->GetVertices(),
			polygonB->GetVertices(),
			contactNormal,
			referenceEdge,
			referenceEdgeIndex,
			aHasReferenceEdge);

		ContactPoints contactPoints = ContactPoints();
		contactPoints.NumberOfContactPoints = 2;

		FindPolygonVsPolygonContactPoints(*polygonA,
			*polygonB,
			contactNormal,
			aHasReferenceEdge,
			referenceEdge,
			referenceEdgeIndex,
			contactPoints.Points,
			contactPoints.NumberOfContactPoints);

		DoDebug(
		ECS::Systems::DebugDrawSystem* debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
		if (contactPoints.NumberOfContactPoints > 0)
		{
			debugSystem->DrawHollowCircle(contactPoints.Points[0], 10.0f, 255, 240, 0, 255);

			if (contactPoints.NumberOfContactPoints > 1)
			{
				debugSystem->DrawHollowCircle(contactPoints.Points[1], 10.0f, 255, 240, 0, 255);
			}
		}
		);

		manifolds.emplace_back(firstPolygonTransform,
			secondPolygonTransform,
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityA),
			Core::ECS::ECSManager::GetInstance().GetComponent<Components::Rigidbody2D>(pair.EntityB),
			std::forward<glm::vec2>(contactNormal),
			penetrationDepth,
			contactPoints);
	}
}

