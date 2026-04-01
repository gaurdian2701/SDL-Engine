#pragma once
#include "../System.h"
#include "NarrowPhase.h"
#include "Solver.h"

namespace Core::ECS::Systems
{
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		void RegisterInterestedComponents() override;
		void ProcessInitializationQueue() override;
		void BeginSystem() override;
		void UpdateSystem(const float deltaTime) override;
		void UpdateRigidbodies(const float physicsTimeStep);

	private:
		std::vector<Physics::CollisionManifold> m_collisionManifolds = std::vector<Physics::CollisionManifold>();
		glm::vec2 m_gravity = glm::vec2(0.0f, -9.81f);
		float accumulator = 0.0f;
		const float m_timeStep = 1 / 60.0f;
		Physics::NarrowPhase m_narrowPhase = Physics::NarrowPhase();
		Physics::Solver m_solver = Physics::Solver();
	};
}
