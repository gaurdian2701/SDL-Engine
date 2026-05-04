#include "../../../include/Core/Physics/NaiveBroadPhase.h"
#include "Components/PolygonCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "../../../include/Core/Physics/ShapeOverlapFunctions.h"
#include "Core/Physics/CollisionPair.h"

void Core::Physics::NaiveBroadPhase::GeneratePairs(std::vector<PhysicsData::CollisionPair> &collisionPairs)
{
	collisionPairs.clear();

	//Do circles vs circles first
	auto [circleID1, circleView] = Core::ECS::ECSManager::GetInstance().GetView<Components::Transform, Components::CircleCollider2D>();

	for (; circleID1 != circleView.end(); ++circleID1)
	{
		auto circleCollider1 = Core::ECS::ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleID1);

		for (auto circleID2 = circleID1 + 1; circleID2 != circleView.end(); ++circleID2)
		{
		auto circleCollider2 = Core::ECS::ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleID2);

			if (circleCollider1->GetAABB().Overlaps(circleCollider2->GetAABB()))
			{
				collisionPairs.push_back(Core::Physics::PhysicsData::CollisionPair(*circleID1, *circleID2, PhysicsData::CircleVsCircle));
			}
		}
	}

	//Do Polygons vs Polygons followed by Polygons vs circles
	auto [polygonID1, polygonView] = Core::ECS::ECSManager::GetInstance().GetView<Components::Transform, Components::PolygonCollider2D>();

	for (; polygonID1 != polygonView.end(); ++polygonID1)
	{
		auto polygonCollider1 = Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(*polygonID1);

		//Do polygon vs polygon
		for (auto polygonID2 = polygonID1 + 1; polygonID2 != polygonView.end(); ++polygonID2)
		{
			auto polygonCollider2 = Core::ECS::ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(*polygonID2);
			if (polygonCollider1->GetAABB().Overlaps(polygonCollider2->GetAABB()))
			{
				collisionPairs.push_back(PhysicsData::CollisionPair(*polygonID1, *polygonID2, PhysicsData::PolygonVsPolygon));
			}
		}

		//Do polygon vs circle
		for (circleID1 = circleView.begin()+1; circleID1 != circleView.end(); ++circleID1)
		{
			auto circleCollider = Core::ECS::ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleID1);
			if (polygonCollider1->GetAABB().Overlaps(circleCollider->GetAABB()))
			{
				collisionPairs.push_back(PhysicsData::CollisionPair(*polygonID1, *circleID1, PhysicsData::PolygonVsCircle));
			}
		}
	}
}
