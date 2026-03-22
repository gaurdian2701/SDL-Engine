#include "Core/ECS/Systems/Debug/DebugDrawSystem.h"
#include "Components/BoxCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Transform.h"
#include "Core/HelperFunctions.h"
#include "Core/ECS/ECSManager.h"

void Core::ECS::Systems::DebugDrawSystem::RegisterInterestedComponents()
{
    ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Components::Transform,
    Components::BoxCollider2D, Components::CircleCollider2D>(this);
}

void Core::ECS::Systems::DebugDrawSystem::UpdateSystem(const float deltaTime)
{
    //Draw AABB Colliders
    ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::BoxCollider2D>(
        [&](const Components::Transform* transform, Components::BoxCollider2D* box2D)
        {
            if (box2D->IsColliding)
            {
                SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(),
                    255, 0, 0, 255);
            }
            else
            {
                SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(),
                    0, 255, 0, 255);
            }

            DrawBox(box2D->GetTopLeftPoint(),
                box2D->GetTopRightPoint(),
                box2D->GetBottomRightPoint(),
                box2D->GetBottomLeftPoint());
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
            this->DrawHollowCircle(transform->Position, circleCollider->Radius);
        });
}

void Core::ECS::Systems::DebugDrawSystem::DrawBox(const glm::vec2& topLeft,
    const glm::vec2& topRight,
    const glm::vec2& bottomRight,
    const glm::vec2& bottomLeft) const
{
    glm::vec2 topLeftScreenCoordinates = WorldToScreen(topLeft);
    glm::vec2 topRightScreenCoordinates = WorldToScreen(topRight);
    glm::vec2 bottomRightScreenCoordinates = WorldToScreen(bottomRight);
    glm::vec2 bottomLeftScreenCoordinates = WorldToScreen(bottomLeft);

    m_boxDrawPoints[0] = SDL_FPoint(topLeftScreenCoordinates.x, topLeftScreenCoordinates.y);
    m_boxDrawPoints[1] = SDL_FPoint(topRightScreenCoordinates.x, topRightScreenCoordinates.y);
    m_boxDrawPoints[2] = SDL_FPoint(bottomRightScreenCoordinates.x, bottomRightScreenCoordinates.y);
    m_boxDrawPoints[3] = SDL_FPoint(bottomLeftScreenCoordinates.x, bottomLeftScreenCoordinates.y);
    m_boxDrawPoints[4] = m_boxDrawPoints[0];

    SDL_RenderLines(Application::GetCoreInstance().GetMainRenderer(), m_boxDrawPoints, 5);
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
void Core::ECS::Systems::DebugDrawSystem::DrawHollowCircle(const glm::vec2& worldPosition, float radius)
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

