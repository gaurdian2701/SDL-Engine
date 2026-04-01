#include "Core/ECS/Systems/Debug/DebugDrawSystem.h"
#include "Components/BoxCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/Debug/DebugDrawHelpers.h"

void Core::ECS::Systems::DebugDrawSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Components::Transform,
                                                                    Components::BoxCollider2D,
                                                                    Components::CircleCollider2D>(this);
}

void Core::ECS::Systems::DebugDrawSystem::UpdateSystem(const float deltaTime)
{
    //Draw Box Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::BoxCollider2D>(
        [&](const Components::Transform* transform, const Components::BoxCollider2D* box2D)
        {
            if (box2D->IsColliding)
            {
                Debug::DebugDrawHelpers::DrawDebugBox(box2D->GetTopLeftPoint(),
                                                      box2D->GetTopRightPoint(),
                                                      box2D->GetBottomRightPoint(),
                                                      box2D->GetBottomLeftPoint(),
                                                      255, 0, 0, 255);
            }
            else
            {
                Debug::DebugDrawHelpers::DrawDebugBox(box2D->GetTopLeftPoint(),
                                                      box2D->GetTopRightPoint(),
                                                      box2D->GetBottomRightPoint(),
                                                      box2D->GetBottomLeftPoint(),
                                                      0, 255, 0, 255);
            }
        });

    //Draw Circle Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
        [&](const Components::Transform* transform, const Components::CircleCollider2D* circleCollider)
        {
            if (circleCollider->IsColliding)
            {
                Debug::DebugDrawHelpers::DrawDebugHollowCircle(transform->Position, circleCollider->Radius,
                                                               255, 0, 0, 255);
            }
            else
            {
                Debug::DebugDrawHelpers::DrawDebugHollowCircle(transform->Position, circleCollider->Radius,
                                                               0, 255, 0, 255);
            }
        });
}
