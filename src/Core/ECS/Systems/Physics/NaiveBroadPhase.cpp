#include "Core/ECS/Systems/Physics/NaiveBroadPhase.h"
#include "Components/PolygonCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/Physics/CollisionChecks.h"
#include "Core/ECS/Systems/Physics/CollisionPair.h"

void Core::ECS::Systems::Physics::NaiveBroadPhase::GeneratePairs(std::vector<CollisionPair> &collisionPairs)
{
	collisionPairs.clear();

	//Do circles vs circles first
	auto [circleID1, circleView] = ECSManager::GetInstance().GetView<Components::Transform, Components::CircleCollider2D>();

	for (circleID1; circleID1 != circleView.end(); ++circleID1)
	{
		auto circleCollider1 = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleID1);

		for (auto circleID2 = circleID1 + 1; circleID2 != circleView.end(); ++circleID2)
		{
		auto circleCollider2 = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleID2);

			if (CollisionChecks::AABBVsAABB(circleCollider1->GetAABB(), circleCollider2->GetAABB()))
			{
				collisionPairs.push_back(CollisionPair(*circleID1, *circleID2, CircleVsCircle));
			}
		}
	}

	//Do Polygons vs Polygons followed by Polygons vs circles
	auto [polygonID1, polygonView] = ECSManager::GetInstance().GetView<Components::Transform, Components::PolygonCollider2D>();

	for (polygonID1; polygonID1 != polygonView.end(); ++polygonID1)
	{
		auto polygonCollider1 = ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(*polygonID1);

		//Do polygon vs polygon
		for (auto polygonID2 = polygonID1 + 1; polygonID2 != polygonView.end(); ++polygonID2)
		{
			auto polygonCollider2 = ECSManager::GetInstance().GetComponent<Components::PolygonCollider2D>(*polygonID2);
			if (CollisionChecks::AABBVsAABB(polygonCollider1->GetAABB(), polygonCollider2->GetAABB()))
			{
				collisionPairs.push_back(CollisionPair(*polygonID1, *polygonID2));
			}
		}

		//Do polygon vs circle
		for (auto circleID1 = circleView.begin()+1; circleID1 != circleView.end(); ++circleID1)
		{
			auto circleCollider = ECSManager::GetInstance().GetComponent<Components::CircleCollider2D>(*circleID1);
			if (CollisionChecks::AABBVsAABB(polygonCollider1->GetAABB(), circleCollider->GetAABB()))
			{
				collisionPairs.push_back(CollisionPair(*polygonID1, *circleID1, PolygonVsCircle));
			}
		}
	}
}
