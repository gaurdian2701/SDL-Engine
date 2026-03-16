#pragma once
#include "System.h"


namespace Core::ECS::Systems
{
    class RenderingSystem : public System
    {
    public:
        RenderingSystem() = default;
        ~RenderingSystem() override = default;

        void RegisterInterestedComponents() override;
        void BeginSystem() override{}
        void ProcessInitializationQueue() override{}
        void UpdateSystem(const float deltaTime) override;
        void EndSystem() override;
    };
}
