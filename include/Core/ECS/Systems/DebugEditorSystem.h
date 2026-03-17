#pragma once
#include "System.h"

namespace Core::ECS::Systems
{
    class DebugEditorSystem : public System
    {
    public:
        DebugEditorSystem() = default;
        ~DebugEditorSystem() override = default;

        void ProcessInitializationQueue() override{}
        void RegisterInterestedComponents() override;
        void UpdateSystem(const float deltaTime) override;
    };
}
