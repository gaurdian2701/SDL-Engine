#include "../../../../../include/Core/ECS/Systems/DebugDrawSystem.h"
#include "Components/PolygonCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "../../../../../include/Core/Debug/DebugDrawHelpers.h"

void Core::ECS::Systems::DebugDrawSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Components::Transform,
                                                                    Components::PolygonCollider2D,
                                                                    Components::CircleCollider2D>(this);
}

void Core::ECS::Systems::DebugDrawSystem::DrawFromDebugDrawList()
{
    for (auto& drawData : m_drawList)
    {
        switch (drawData.shapeType)
        {
            default:
            case ShapeType::CIRCLE:
            {
                Debug::DebugDrawHelpers::DrawDebugHollowCircle(drawData.shapeData.circle.CenterWorldPos,
                    drawData.shapeData.circle.radius,
                    drawData.r,
                    drawData.g,
                    drawData.b,
                    drawData.a);
                break;
            }
            case ShapeType::POLYGON:
            {
                Debug::DebugDrawHelpers::DrawDebugPolygon(*drawData.shapeData.polygon.Points,
                    drawData.r,
                    drawData.g,
                    drawData.b,
                    drawData.a);
                break;
            }
        }
    }
    m_drawList.clear();
}

void Core::ECS::Systems::DebugDrawSystem::UpdateSystem(const float deltaTime)
{
    //Batch draw calls for box colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::PolygonCollider2D>(
        [&](const Components::Transform* transform, const Components::PolygonCollider2D* polygon2D)
        {
            if (polygon2D->IsColliding)
            {
                m_drawList.emplace_back(DebugDrawData(PolygonDrawData(&polygon2D->GetVertices()), 255, 0, 0, 255));
            }
            else
            {
                m_drawList.emplace_back(DebugDrawData(PolygonDrawData(&polygon2D->GetVertices()), 0, 255, 0, 255));
            }
        });

    //batch draw calls for circle colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
        [&](const Components::Transform* transform, const Components::CircleCollider2D* circleCollider)
        {
            if (circleCollider->IsColliding)
            {
                m_drawList.emplace_back(DebugDrawData(CircleDrawData(transform->Position, circleCollider->GetRadius()),
                    255, 0, 0, 255));
            }
            else
            {
                m_drawList.emplace_back(DebugDrawData(CircleDrawData(transform->Position, circleCollider->GetRadius()),
                    0, 255, 0, 255));
            }
        });
}

void Core::ECS::Systems::DebugDrawSystem::DrawHollowCircle(const glm::vec2 &centerWorldPos, const float radius, float r, float g, float b, float a)
{
    m_drawList.emplace_back(DebugDrawData(CircleDrawData(centerWorldPos, radius), r, g, b, a));
}

void Core::ECS::Systems::DebugDrawSystem::DrawDebugPolygon(std::vector<glm::vec2>& points,
    float r, float g, float b, float a)
{
    m_drawList.emplace_back(DebugDrawData(PolygonDrawData(&points), r, g, b, a));
}


