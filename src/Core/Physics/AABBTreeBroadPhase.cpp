#include "Core/Physics/AABBTreeBroadPhase.h"
#include "vec2.hpp"
#include "Application/Application.h"
#include "Components/CircleCollider2D.h"
#include "Components/PolygonCollider2D.h"
#include "Core/ScreenHelperFunctions.h"
#include "Core/DataStructures/AABBQuadTree.h"
#include "Core/ECS/ECSManager.h"
#include "Core/Physics/CollisionPair.h"
#include "Core/Physics/ShapeOverlapFunctions.h"

Core::Physics::AABBTreeBroadPhase::AABBTreeBroadPhase()
{
    auto &app = Application::GetCoreInstance();
    m_topRightBoundary = ScreenHelperFunctions::ScreenToWorld(glm::vec2(app.GetScreenWidth(), 0.0f));
    m_bottomLeftBoundary = ScreenHelperFunctions::ScreenToWorld(glm::vec2(0.0f, app.GetScreenHeight()));
}

void Core::Physics::AABBTreeBroadPhase::GeneratePairs(std::vector<PhysicsData::CollisionPair> &collisionPairs)
{
    collisionPairs.clear();
    //construct quadtree from entities
    auto &ecsManager = Core::ECS::ECSManager::GetInstance();
    auto [circleStart, circleView] = ecsManager.GetView<Components::CircleCollider2D>();
    auto [polygonStart, polygonView] = ecsManager.GetView<Components::PolygonCollider2D>();
    aabbTree = new DataStructures::AABBQuadTree(m_topRightBoundary, m_bottomLeftBoundary, 0);

    static std::vector<Components::AABB *> allAABBs = std::vector<Components::AABB *>();
    allAABBs.reserve(ecsManager.GetMaxEntityCount());
    static std::vector<Components::AABB *> aabbsToCheckAgainst = std::vector<Components::AABB *>();

    for (auto circleIt = circleStart; circleIt != circleView.end(); ++circleIt)
    {
        auto circle = ecsManager.GetComponent<Components::CircleCollider2D>(*circleIt);
        circle->GetAABB().Entity = *circleIt;
        aabbTree->Insert(circle->GetAABB());
        allAABBs.push_back(&circle->GetAABB());
    }

    for (auto polygonIt = polygonStart; polygonIt != polygonView.end(); ++polygonIt)
    {
        auto polygon = ecsManager.GetComponent<Components::PolygonCollider2D>(*polygonIt);
        polygon->GetAABB().Entity = *polygonIt;
        aabbTree->Insert(polygon->GetAABB());
        allAABBs.push_back(&polygon->GetAABB());
    }

    aabbsToCheckAgainst.reserve(20);

    for (auto& aabb1 : allAABBs)
    {
        aabbTree->GetClosest(*aabb1, aabbsToCheckAgainst);
        for (auto& aabb2 : aabbsToCheckAgainst)
        {
            if (aabb1 != aabb2 && aabb1 < aabb2)
            {
                PhysicsData::CollisionType CollisionType;
                if (aabb1->Collider == Components::ColliderType::POLYGON)
                {
                    if (aabb2->Collider == Components::ColliderType::POLYGON)
                    {
                        CollisionType = PhysicsData::PolygonVsPolygon;
                    }
                    else
                    {
                        CollisionType = PhysicsData::PolygonVsCircle;
                    }
                    collisionPairs.emplace_back(aabb1->Entity, aabb2->Entity, CollisionType);
                }
                else
                {
                    if (aabb2->Collider == Components::ColliderType::POLYGON)
                    {
                        CollisionType = PhysicsData::PolygonVsCircle;
                        collisionPairs.emplace_back(aabb2->Entity, aabb1->Entity, CollisionType);
                    }
                    else
                    {
                        CollisionType = PhysicsData::CircleVsCircle;
                        collisionPairs.emplace_back(aabb1->Entity, aabb2->Entity, CollisionType);
                    }
                }
            }
        }

        aabbsToCheckAgainst.clear();
    }

    allAABBs.clear();
    delete aabbTree;
}
