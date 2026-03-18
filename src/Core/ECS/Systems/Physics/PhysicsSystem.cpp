#include "Core/ECS/Systems/Physics/PhysicsSystem.h"
#include "Components/AABB2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/ECS/ECSManager.h"
#include "Core/ECS/Systems/Physics/CollisionChecks.h"

void Core::ECS::Systems::PhysicsSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem
        <Components::Transform, Components::AABB2D>(this);
}

void Core::ECS::Systems::PhysicsSystem::BeginSystem()
{
}

void Core::ECS::Systems::PhysicsSystem::UpdateSystem(const float deltaTime)
{
    ECSManager::GetInstance().ForEachUsingComponents<Components::AABB2D>(
        [&](Components::AABB2D* box)
        {
            box->IsColliding = false;
        });

    ECSManager::GetInstance().ForEachUsingComponents<Components::CircleCollider2D>(
    [&](Components::CircleCollider2D* circleCollider)
        {
            circleCollider->IsColliding = false;
        });

    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::AABB2D>(
        [&](Components::Transform* firstTransform, Components::AABB2D* firstBox)
        {
            ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::AABB2D>(
                [&](Components::Transform* secondTransform, Components::AABB2D* secondBox)
                {
                    if (firstBox != secondBox)
                    {
                        if (AABBvsAABB(firstTransform->WorldPosition,
                                       secondTransform->WorldPosition,
                                       *firstBox,
                                       *secondBox))
                        {
                            firstBox->IsColliding = true;
                            secondBox->IsColliding = true;
                        }
                    }
                });
        });

    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
    [&](Components::Transform* firstTransform, Components::CircleCollider2D* firstCircle)
    {
        ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
            [&](Components::Transform* secondTransform, Components::CircleCollider2D* secondCircle)
            {
                if (firstCircle != secondCircle)
                {
                    if (CirclevsCircle(firstTransform->WorldPosition,
                                   secondTransform->WorldPosition,
                                   *firstCircle,
                                   *secondCircle))
                    {
                        firstCircle->IsColliding = true;
                        secondCircle->IsColliding = true;
                    }
                }
            });
    });
}
