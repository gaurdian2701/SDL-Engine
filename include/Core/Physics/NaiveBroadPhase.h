#pragma once
#include "BroadPhase.h"

namespace Core::Physics
{
	class NaiveBroadPhase : public BroadPhase
	{
	public:
		NaiveBroadPhase() = default;
		~NaiveBroadPhase() override = default;

		virtual void GeneratePairs(std::vector<PhysicsData::CollisionPair>& collisionPairs) override;
	};
}
