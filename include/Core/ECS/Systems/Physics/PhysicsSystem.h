#pragma once
#include "../System.h"

namespace Core::ECS::Systems::Physics
{
	class NarrowPhase;
}

namespace Core::ECS::Systems
{
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void RegisterInterestedComponents() override;
		void ProcessInitializationQueue() override{}
		void BeginSystem() override;
		void UpdateSystem(const float deltaTime) override;
		void UpdateRigidbodies(const float deltaTime);

	private:
		glm::vec2 m_gravity = glm::vec2(0.0f, -9.81f);
		Physics::NarrowPhase* m_narrowPhase = nullptr;
	};
}
