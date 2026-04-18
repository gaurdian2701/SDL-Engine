#pragma once
#include <vector>
#include "ContactManifold.h"

namespace Core::Physics::PhysicsData
{
	struct ContactManifold;
}

namespace Core::Physics
{
	class Solver
	{
	public:
		Solver() = default;
		~Solver() = default;

		void Solve(std::vector<PhysicsData::ContactManifold>& manifolds, const float physicsTimeStep);

	private:
		const std::uint8_t m_solverIterations = 4;
	};
}
