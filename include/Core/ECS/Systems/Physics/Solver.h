#pragma once
#include <vector>
#include "CollisionManifold.h"

namespace Core::ECS::Systems::Physics
{
	class Solver
	{
	public:
		Solver() = default;
		~Solver() = default;

		void Solve(const std::vector<CollisionManifold>& manifolds);
	};
}
