#include "Core/ECS/Systems/Debug/DebugDrawSystem.h"
#include "Components/AABB2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/HelperFunctions.h"
#include "Core/ECS/ECSManager.h"

void Core::ECS::Systems::DebugDrawSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Components::Transform,
    Components::AABB2D, Components::CircleCollider2D>(this);
}

void Core::ECS::Systems::DebugDrawSystem::UpdateSystem(const float deltaTime)
{
    SDL_FRect drawRect = SDL_FRect();

    //Draw AABB Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::AABB2D>(
        [&](const Components::Transform* transform, Components::AABB2D* aabb2D)
        {
            glm::vec2 screenCoordinates = WorldToScreen(transform->WorldPosition);
            drawRect = SDL_FRect(screenCoordinates.x - aabb2D->HalfExtents.x,
                screenCoordinates.y - aabb2D->HalfExtents.y,
                aabb2D->HalfExtents.x * 2.0f,
                aabb2D->HalfExtents.y * 2.0f);

            if (aabb2D->IsColliding)
            {
                SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(),
                    255, 0, 0, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(),
                    0, 255, 0, 255);
            }

            SDL_RenderRect(Application::GetCoreInstance().GetMainRenderer(), &drawRect);
        });

    //Draw Circle Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::CircleCollider2D>(
        [&](Components::Transform* transform, Components::CircleCollider2D* circleCollider)
        {
            if (circleCollider->IsColliding)
            {
                SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(),
                    255, 0, 0, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(),
                    0, 255, 0, 255);
            }
            this->DrawHollowCircle(transform->WorldPosition, circleCollider->Radius);
        });
}

void Core::ECS::Systems::DebugDrawSystem::DrawCirclePoints(float xc, float yc, float x, float y)
{
    m_circleDrawPoints[0] = SDL_FPoint(xc+x, yc+y);
    m_circleDrawPoints[1] = SDL_FPoint(xc-x, yc+y);
    m_circleDrawPoints[2] = SDL_FPoint(xc+x, yc-y);
    m_circleDrawPoints[3] = SDL_FPoint(xc-x, yc-y);
    m_circleDrawPoints[4] = SDL_FPoint(xc+y, yc+x);
    m_circleDrawPoints[5] = SDL_FPoint(xc-y, yc+x);
    m_circleDrawPoints[6] = SDL_FPoint(xc+y, yc-x);
    m_circleDrawPoints[7] = SDL_FPoint(xc-y, yc-x);

    SDL_RenderPoints(Application::GetCoreInstance().GetMainRenderer(), m_circleDrawPoints, 8);
}

//Uses Bresenham's Circle Drawing Algorithm
void Core::ECS::Systems::DebugDrawSystem::DrawHollowCircle(glm::vec2& worldPosition, float radius)
{
    glm::vec2 screenPosition = WorldToScreen(worldPosition);
    int x = 0, y = radius;
    int d = 3 - 2 * radius;
    DrawCirclePoints(screenPosition.x, screenPosition.y, x, y);

    while (y >= x)
    {
        if (d > 0)
        {
            y--;
            d = d + 4 * (x-y) + 10;
        }
        else
        {
            d = d + 4 * x + 6;
        }
        x++;

        DrawCirclePoints(screenPosition.x, screenPosition.y, x, y);
    }
}

