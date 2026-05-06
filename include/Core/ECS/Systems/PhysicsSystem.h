#pragma once
#include "Core/Physics/Solver.h"
#include "System.h"
#include "Core/Physics/CollisionPair.h"
#include "Core/Physics/NarrowPhase.h"

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
		PhysicsSystem();
		~PhysicsSystem() override = default;

		void RegisterInterestedComponents() override;
		void OnComponentAdded(const std::uint32_t entityID) override;
		void BeginSystem() override;
		void UpdateSystem(const float deltaTime) override;

	private:
		void IntegrateVelocities(const float physicsTimeStep);
		void UpdateCollidersAndRigidbodies();
		void IntegratePositions(const float physicsTimeStep);

	public:
		double m_TimeTakenForPhysicsStep = 0.0f;
		std::string m_broadPhaseUsedText;

	private:
		std::vector<Physics::PhysicsData::ContactManifold> m_collisionManifolds =
			std::vector<Physics::PhysicsData::ContactManifold>();

		std::vector<Physics::PhysicsData::CollisionPair> m_collisionPairs =
			std::vector<Physics::PhysicsData::CollisionPair>();

		const glm::vec2 m_gravity = glm::vec2(0.0f, -9.81f * 5.0f);

		Physics::BroadPhase* m_currentBroadPhase = nullptr;
		Physics::BroadPhase* m_naiveBroadPhase = nullptr;
		Physics::BroadPhase* m_quadTreeBroadPhase = nullptr;

		static PhysicsSystem* m_instance;
		float m_accumulator = 0.0f;
		const float m_timeStep = 1 / 60.0f;
		Physics::NarrowPhase m_narrowPhase = Physics::NarrowPhase();
		Physics::Solver m_solver = Physics::Solver();

		friend ECSManager;
	};
}
