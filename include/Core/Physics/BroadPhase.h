#pragma once
#include <vector>


namespace Core::Physics
{
	namespace PhysicsData
	{
		struct CollisionPair;
	}

	class BroadPhase
	{
	public:
		BroadPhase() = default;
		virtual ~BroadPhase() = default;

		virtual void GeneratePairs(std::vector<PhysicsData::CollisionPair>& collisionPairs) = 0;
	};
}
