#pragma once
#include "vec2.hpp"
#include "Core/ScreenHelperFunctions.h"
#include "SDL3/SDL_rect.h"

namespace Debug::DebugDrawHelpers
{
    static inline void DrawDebugPolygon(const std::vector<glm::vec2>& points,
        float r,
        float g,
        float b,
        float a)
    {
        std::vector<SDL_FPoint> polygonDrawPoints = std::vector<SDL_FPoint>(points.size() + 1);
        glm::vec2 screenCoordinates = Core::ScreenHelperFunctions::WorldToScreen(glm::vec2(0.0f));

        int pointIndex = 0;

        for (pointIndex; pointIndex < points.size(); pointIndex++)
        {
            screenCoordinates = Core::ScreenHelperFunctions::WorldToScreen(points[pointIndex]);
            polygonDrawPoints[pointIndex] = SDL_FPoint(screenCoordinates.x, screenCoordinates.y);
        }
        polygonDrawPoints[pointIndex] = polygonDrawPoints[0];

        SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), r, g, b, a);
        SDL_RenderLines(Application::GetCoreInstance().GetMainRenderer(), polygonDrawPoints.data(), points.size()+1);
    }

    static inline void DrawCirclePoints(float xc, float yc, float x, float y)
    {
        static std::vector<SDL_FPoint> m_circleDrawPoints = std::vector<SDL_FPoint>(8);

        m_circleDrawPoints[0] = SDL_FPoint(xc+x, yc+y);
        m_circleDrawPoints[1] = SDL_FPoint(xc-x, yc+y);
        m_circleDrawPoints[2] = SDL_FPoint(xc+x, yc-y);
        m_circleDrawPoints[3] = SDL_FPoint(xc-x, yc-y);
        m_circleDrawPoints[4] = SDL_FPoint(xc+y, yc+x);
        m_circleDrawPoints[5] = SDL_FPoint(xc-y, yc+x);
        m_circleDrawPoints[6] = SDL_FPoint(xc+y, yc-x);
        m_circleDrawPoints[7] = SDL_FPoint(xc-y, yc-x);

        SDL_RenderPoints(Application::GetCoreInstance().GetMainRenderer(), m_circleDrawPoints.data(), 8);
    }

    //Uses Bresenham's Circle Drawing Algorithm
    static inline void DrawDebugHollowCircle(const glm::vec2& centerWorldPos,
        const float radius,
        float r,
        float g,
        float b,
        float a)
    {
        glm::vec2 screenPosition = Core::ScreenHelperFunctions::WorldToScreen(centerWorldPos);
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
