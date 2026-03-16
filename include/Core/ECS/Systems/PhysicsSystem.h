#pragma once
#include "System.h"

namespace Core::ECS::Systems
{
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void RegisterInterestedComponents() override;
		void ProcessInitializationQueue() override{}
		void UpdateSystem(const float deltaTime) override;
	};
}
