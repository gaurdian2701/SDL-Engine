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
    };
}
