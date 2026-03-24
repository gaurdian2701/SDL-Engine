#pragma once
#include "vec2.hpp"
#include "Core/HelperFunctions.h"
#include "SDL3/SDL_rect.h"

namespace Debug::DebugDrawHelpers
{
    static inline void DrawDebugBox(const glm::vec2& topLeft,
    const glm::vec2& topRight,
    const glm::vec2& bottomRight,
    const glm::vec2& bottomLeft,
    float r,
    float g,
    float b,
    float a)
    {
        static SDL_FPoint* m_boxDrawPoints = new SDL_FPoint[5];

        glm::vec2 topLeftScreenCoordinates = Core::WorldToScreen(topLeft);
        glm::vec2 topRightScreenCoordinates = Core::WorldToScreen(topRight);
        glm::vec2 bottomRightScreenCoordinates = Core::WorldToScreen(bottomRight);
        glm::vec2 bottomLeftScreenCoordinates = Core::WorldToScreen(bottomLeft);

        m_boxDrawPoints[0] = SDL_FPoint(topLeftScreenCoordinates.x, topLeftScreenCoordinates.y);
        m_boxDrawPoints[1] = SDL_FPoint(topRightScreenCoordinates.x, topRightScreenCoordinates.y);
        m_boxDrawPoints[2] = SDL_FPoint(bottomRightScreenCoordinates.x, bottomRightScreenCoordinates.y);
        m_boxDrawPoints[3] = SDL_FPoint(bottomLeftScreenCoordinates.x, bottomLeftScreenCoordinates.y);
        m_boxDrawPoints[4] = m_boxDrawPoints[0];

        SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), r, g, b, a);
        SDL_RenderLines(Application::GetCoreInstance().GetMainRenderer(), m_boxDrawPoints, 5);
    }

    static inline void DrawCirclePoints(float xc, float yc, float x, float y)
    {
        static SDL_FPoint* m_circleDrawPoints = new SDL_FPoint[8];

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
    static inline void DrawDebugHollowCircle(const glm::vec2& centerWorldPos,
        const float radius,
        float r,
        float g,
        float b,
        float a)
    {
        glm::vec2 screenPosition = Core::WorldToScreen(centerWorldPos);
        int x = 0, y = radius;
        int d = 3 - 2 * radius;

        SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), r, g, b, a);
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
}
