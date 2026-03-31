#pragma once
#include <vector>
#include "CollisionManifold.h"

namespace Components
{
	struct Transform;
	struct Rigidbody2D;
}

namespace Core::ECS::Systems::Physics
{
	class NarrowPhase
	{
	public:
		NarrowPhase() = default;
		~NarrowPhase() = default;

		void GenerateManifolds(std::vector<CollisionManifold>& manifolds);

	private:
		void GenerateBoxCollisionManifolds(std::vector<CollisionManifold>& manifolds);
		void GenerateCircleCollisionManifolds(std::vector<CollisionManifold>& manifolds);
	};
}
