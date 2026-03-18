#pragma once
#include "Components/AABB2D.h"
#include "Components/Transform.h"

static inline bool AABBvsAABB(const glm::vec2& pos1,
    const glm::vec2& pos2,
    const Components::AABB2D& box1,
    const Components::AABB2D& box2)
{
    //Using SAT(Separating Axis Theorem)
    //box1's maxX is lesser than box2's minX?
    if (pos1.x + box1.HalfExtents.x < pos2.x - box2.HalfExtents.x ||
        pos1.x - box1.HalfExtents.x > pos2.x + box2.HalfExtents.x)
    {
        return false;
    }
    //box1's maxY is lesser than box2's minY?
    if (pos1.y + box1.HalfExtents.y < pos2.y - box2.HalfExtents.y ||
        pos1.y - box1.HalfExtents.y > pos2.y + box2.HalfExtents.y)
    {
        return false;
    }

    return true;
}

static inline bool CirclevsCircle(const glm::vec2& center1,
    const glm::vec2& center2,
    const Components::CircleCollider2D& circle1,
    const Components::CircleCollider2D& circle2)
{
    float radiusSum = circle1.Radius + circle2.Radius;
    radiusSum *= radiusSum;
    return radiusSum > (center1.x - center2.x) * (center1.x - center2.x) +
        (center1.y - center2.y) * (center1.y - center2.y);
}