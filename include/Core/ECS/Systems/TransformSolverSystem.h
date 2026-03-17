#pragma once
#include "System.h"

namespace Components
{
    struct Transform;
}

namespace Core::ECS::Systems
{
    class TransformSolverSystem : public System
    {
    public:
        TransformSolverSystem() = default;
        ~TransformSolverSystem() override = default;

        void RegisterInterestedComponents() override;
        void ProcessInitializationQueue() override{}
        void BeginSystem() override{}
        void UpdateSystem(const float deltaTime) override;
        void EndSystem() override{}

    private:
        inline void SolveTransform(Components::Transform* someTransform);
    };
}
