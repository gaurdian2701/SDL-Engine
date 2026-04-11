#pragma once
#include "NaiveBroadPhase.h"
#include "../System.h"
#include "NarrowPhase.h"
#include "Solver.h"
#include "CollisionPair.h"

namespace Core::ECS::Systems
{
	namespace Physics
	{
		class BroadPhase;
	}
	class PhysicsSystem : public System
	{
	public:
		PhysicsSystem() = default;
		~PhysicsSystem() override = default;

		static PhysicsSystem* GetInstance();
		void RegisterInterestedComponents() override;
		void BeginSystem() override;
		void UpdateSystem(const float deltaTime) override;

	private:
		void UpdateColliders();
		void UpdateRigidbodies(const float physicsTimeStep);

	private:
		std::vector<Physics::CollisionManifold> m_collisionManifolds = std::vector<Physics::CollisionManifold>();
		std::vector<CollisionPair> m_collisionPairs = std::vector<CollisionPair>();
		const glm::vec2 m_gravity = glm::vec2(0.0f, -9.81f);
		Physics::BroadPhase* m_broadPhase = nullptr;
		static PhysicsSystem* m_instance;
		float accumulator = 0.0f;
		const float m_timeStep = 1 / 60.0f;
		Physics::NarrowPhase m_narrowPhase = Physics::NarrowPhase();
		Physics::Solver m_solver = Physics::Solver();

		friend ECSManager;
	};
}
