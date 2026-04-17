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

void Core::ECS::Systems::DebugDrawSystem::UpdateSystem(const float deltaTime)
{
    //Draw Box Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::PolygonCollider2D>(
        [&](const Components::Transform* transform, const Components::PolygonCollider2D* polygon2D)
        {
            if (polygon2D->IsColliding)
            {
                Debug::DebugDrawHelpers::DrawDebugPolygon(polygon2D->GetPoints(), 255, 0, 0, 255);
            }
            else
            {
                Debug::DebugDrawHelpers::DrawDebugPolygon(polygon2D->GetPoints(), 0, 255, 0, 255);
            }
        });

    //Draw Circle Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
        [&](const Components::Transform* transform, const Components::CircleCollider2D* circleCollider)
        {
            if (circleCollider->IsColliding)
            {
                Debug::DebugDrawHelpers::DrawDebugHollowCircle(transform->Position, circleCollider->GetRadius(),
                                                               255, 0, 0, 255);
            }
            else
            {
                Debug::DebugDrawHelpers::DrawDebugHollowCircle(transform->Position, circleCollider->GetRadius(),
                                                               0, 255, 0, 255);
            }
        });
}
