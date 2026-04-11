#pragma once
#include <vector>

struct CollisionPair;

namespace Core::ECS::Systems::Physics
{
	class BroadPhase
	{
	public:
		BroadPhase() = default;
		virtual ~BroadPhase() = default;

		virtual void GeneratePairs(std::vector<CollisionPair>& collisionPairs) = 0;
	};
}
