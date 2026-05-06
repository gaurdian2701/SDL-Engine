#include "Core/DataStructures/AABBQuadTree.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/DebugDrawSystem.h"
#include "Core/Physics/ShapeOverlapFunctions.h"

void Core::DataStructures::AABBQuadTree::ConstructChildBoundaries()
{
    const float midPointX = (m_nodeBoundary.BottomLeft.x + m_nodeBoundary.TopRight.x) * 0.5f;
    const float midPointY = (m_nodeBoundary.BottomLeft.y + m_nodeBoundary.TopRight.y) * 0.5f;
    const glm::vec2 midPoint(midPointX, midPointY);

    //Top Left
    m_childBoundaries[0].BottomLeft = glm::vec2(m_nodeBoundary.BottomLeft.x, midPointY);
    m_childBoundaries[0].TopRight = glm::vec2(midPointX, m_nodeBoundary.TopRight.y);

    //Top Right
    m_childBoundaries[1].BottomLeft = midPoint;
    m_childBoundaries[1].TopRight = m_nodeBoundary.TopRight;

    //Bottom Left
    m_childBoundaries[2].BottomLeft = m_nodeBoundary.BottomLeft;
    m_childBoundaries[2].TopRight = midPoint;

    //Bottom Right
    m_childBoundaries[3].BottomLeft = glm::vec2(midPointX, m_nodeBoundary.BottomLeft.y);
    m_childBoundaries[3].TopRight = glm::vec2(m_nodeBoundary.TopRight.x, midPointY);
}

void Core::DataStructures::AABBQuadTree::Insert(Components::AABB &aabb)
{
    if (m_currentDepth < m_MaxDepth)
    {
        for (int i = 0; i < 4; i++)
        {
            if (m_childBoundaries[i].Contains(aabb))
            {
                if (m_childTrees[i] == nullptr)
                {
                    m_childTrees[i] = new AABBQuadTree(m_childBoundaries[i].TopRight,
                        m_childBoundaries[i].BottomLeft,
                        m_currentDepth + 1);
                }
                m_childTrees[i]->Insert(aabb);
                return;
            }
        }
    }

    //Unable to insert it in any of the children, so insert it in the current tree
    m_aabbsInTree.push_back(&aabb);
}



void Core::DataStructures::AABBQuadTree::GetClosest(Components::AABB &queriedAABB,
                                                    std::vector<Components::AABB *> &closestAABBs)
{
    //First check for aabbs in the current tree, and add those to the list if any
    for (auto &aabb: m_aabbsInTree)
    {
        if (aabb != nullptr &&
            aabb->Overlaps(queriedAABB))
        {
            closestAABBs.push_back(aabb);
        }
    }

    //Then iterate through all the children
    for (auto currentTree : m_childTrees)
    {
        if (currentTree != nullptr)
        {
            //If child tree is fully within the boundaries of the queried AABB,
            //then add all the children of that child tree to the list
            if (queriedAABB.Contains(currentTree->m_nodeBoundary))
            {
                currentTree->QueryAllAABBsInTree(closestAABBs);
            }
            //Else if it simply overlaps, the AABB, repeat the search function in that child
            else if (currentTree->m_nodeBoundary.Overlaps(queriedAABB))
            {
                currentTree->GetClosest(queriedAABB, closestAABBs);
            }
        }
    }
}

void Core::DataStructures::AABBQuadTree::QueryAllAABBsInTree(std::vector<Components::AABB*>& query)
{
    //Add children in current tree
    for (auto& aabb : m_aabbsInTree)
    {
        query.push_back(aabb);
    }

    //Add children in child trees
    for (auto & m_childTree : m_childTrees)
    {
        if (m_childTree != nullptr)
        {
            m_childTree->QueryAllAABBsInTree(query);
        }
    }
}

DoDebug(void Core::DataStructures::AABBQuadTree::Visualize()
{
    glm::vec2 topLeftBoundary = glm::vec2(m_nodeBoundary.BottomLeft.x, m_nodeBoundary.TopRight.y);
    glm::vec2 bottomRightBoundary = glm::vec2(m_nodeBoundary.TopRight.x, m_nodeBoundary.BottomLeft.y);

    static auto debugSystem = ECS::ECSManager::GetInstance().GetSystem<ECS::Systems::DebugDrawSystem>();
    debugSystem->DrawDebugLine(
        topLeftBoundary,
        m_nodeBoundary.TopRight,
        255, 255, 255, 255); //Top Line

    debugSystem->DrawDebugLine(
        m_nodeBoundary.BottomLeft,
        bottomRightBoundary,
        255, 255, 255, 255); //Bottom Line

    debugSystem->DrawDebugLine(
        m_nodeBoundary.BottomLeft,
        topLeftBoundary,
        255, 255, 255, 255); //Left Line

    debugSystem->DrawDebugLine(
        bottomRightBoundary,
        m_nodeBoundary.TopRight,
        255, 255, 255, 255); //Right Line

    for (auto& childTree : m_childTrees)
    {
        if (childTree != nullptr)
        {
            childTree->Visualize();
        }
    }
}
);
