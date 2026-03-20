#pragma once

static inline bool AABBvsAABB(const glm::vec2& pos1,
    const glm::vec2& pos2,
    const glm::vec2& halfExtents1,
    const glm::vec2& halfExtents2)
{
    //Using SAT(Separating Axis Theorem)
    //box1's maxX is lesser than box2's minX?
    if (pos1.x + halfExtents1.x < pos2.x - halfExtents2.x ||
        pos1.x - halfExtents1.x > pos2.x + halfExtents2.x)
    {
        return false;
    }
    //box1's maxY is lesser than box2's minY?
    if (pos1.y + halfExtents1.y < pos2.y - halfExtents2.y ||
        pos1.y - halfExtents1.y > pos2.y + halfExtents2.y)
    {
        return false;
    }

    return true;
}

static inline bool CirclevsCircle(const glm::vec2& center1,
    const glm::vec2& center2,
    float radiusSum)
{
    radiusSum *= radiusSum;
    return radiusSum > (center1.x - center2.x) * (center1.x - center2.x) +
        (center1.y - center2.y) * (center1.y - center2.y);
}