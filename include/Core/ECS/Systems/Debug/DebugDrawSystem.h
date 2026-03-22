#pragma once
#include "Core/ECS/Systems/System.h"

namespace Core::ECS::Systems
{
    class DebugDrawSystem : public System
    {
    public:
        DebugDrawSystem() = default;
        ~DebugDrawSystem() override = default;

        void RegisterInterestedComponents() override;
        void ProcessInitializationQueue() override{}
        void UpdateSystem(const float deltaTime) override;

    private:
        void DrawBox(const glm::vec2& topLeft,
            const glm::vec2& topRight,
            const glm::vec2& bottomRight,
            const glm::vec2& bottomLeft) const;

        void DrawCirclePoints(float xc, float yc, float x, float y);
        void DrawHollowCircle(const glm::vec2& worldPosition, float radius);

    private:
        SDL_FPoint* m_circleDrawPoints = new SDL_FPoint[8];
        SDL_FPoint* m_boxDrawPoints = new SDL_FPoint[5];
    };
}
