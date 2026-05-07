#pragma once
#include <vector>
#include <vec2.hpp>
#include <SDL3/SDL_render.h>
#include "Components/AABB.h"
#include "DebugStatements.h"

namespace Core::DataStructures
{
    class AABBQuadTree
    {
    public:
        AABBQuadTree(glm::vec2& topRightBoundary, glm::vec2& bottomLeftBoundary, uint8_t currentDepth)
        {
            m_nodeBoundary.BottomLeft = bottomLeftBoundary;
            m_nodeBoundary.TopRight = topRightBoundary;
            m_currentDepth = currentDepth;

            ConstructChildBoundaries();
        }

        ~AABBQuadTree()
        {
            Clear();
        }

        void Insert(Components::AABB& aabb);

        void Clear();

        void GetClosest(Components::AABB& queriedAABB,
            std::vector<Components::AABB*>& closestAABBs);

        void QueryAllAABBsInTree(std::vector<Components::AABB*>& query);

        DoDebug(void Visualize();)

    private:
        void ConstructChildBoundaries();

    public:
        //1 - TopLeft
        //2 - Top Right
        //3 - Bottom Left
        //4 - Bottom Right

        AABBQuadTree* m_childTrees[4] = {nullptr};
        Components::AABB m_childBoundaries[4] = {Components::AABB()};

    private:
        Components::AABB m_nodeBoundary = Components::AABB();
        uint8_t m_MaxDepth = 5;
        uint8_t m_currentDepth = 0;
        std::vector<Components::AABB*> m_aabbsInTree = std::vector<Components::AABB*>();
    };
}
