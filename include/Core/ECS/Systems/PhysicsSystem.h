#pragma once
#include "Core/Physics/NaiveBroadPhase.h"
#include "Core/Physics/NarrowPhase.h"
#include "Core/Physics/Solver.h"
#include "System.h"
#include "Core/Physics/CollisionPair.h"

namespace Core::Physics
{
	class BroadPhase;

	namespace PhysicsData
	{
		struct ContactManifold;
	}
}

namespace Core::ECS::Systems
{
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
		void IntegrateVelocities(const float physicsTimeStep);
		void UpdateColliders();
		void IntegratePositions(const float physicsTimeStep);

	private:
		std::vector<Physics::PhysicsData::ContactManifold> m_collisionManifolds =
			std::vector<Physics::PhysicsData::ContactManifold>();

		std::vector<Physics::PhysicsData::CollisionPair> m_collisionPairs =
			std::vector<Physics::PhysicsData::CollisionPair>();

		const glm::vec2 m_gravity = glm::vec2(0.0f, -9.81f * 5.0f);
		Physics::BroadPhase* m_broadPhase = nullptr;
		static PhysicsSystem* m_instance;
		float accumulator = 0.0f;
		const float m_timeStep = 1 / 60.0f;
		Physics::NarrowPhase m_narrowPhase = Physics::NarrowPhase();
		Physics::Solver m_solver = Physics::Solver();

		friend ECSManager;
	};
}
