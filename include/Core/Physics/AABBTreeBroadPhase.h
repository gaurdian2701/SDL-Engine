#pragma once
#include "BroadPhase.h"
#include "vec2.hpp"

namespace Core::DataStructures
{
    class AABBQuadTree;
}

namespace Core::Physics
{
    class AABBTreeBroadPhase : public BroadPhase
    {
    public:
        AABBTreeBroadPhase();
        ~AABBTreeBroadPhase() override = default;

        void GeneratePairs(std::vector<PhysicsData::CollisionPair> &collisionPairs) override;

    private:
        glm::vec2 m_topRightBoundary = glm::vec2(0.0f);
        glm::vec2 m_bottomLeftBoundary = glm::vec2(0.0f);
        DataStructures::AABBQuadTree* aabbTree = nullptr;
    };
}
