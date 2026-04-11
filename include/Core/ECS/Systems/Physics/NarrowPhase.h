#pragma once
#include <vector>

struct CollisionPair;

namespace Components
{
	struct CircleCollider2D;
	struct PolygonCollider2D;
	struct Transform;
	struct Rigidbody2D;
}

namespace Core::ECS::Systems::Physics
{
	struct CollisionManifold;

	class NarrowPhase
	{
	public:
		NarrowPhase() = default;
		~NarrowPhase() = default;

		void GenerateManifolds(const std::vector<CollisionPair>& collisionPairs, std::vector<CollisionManifold>& manifolds);

	private:
		static void DoCircleVsCircle(const CollisionPair& pair, std::vector<CollisionManifold>& manifolds);
		static void DoPolygonVsCircle(const CollisionPair& pair, std::vector<CollisionManifold>& manifolds);
		static void DoPolygonVsPolygon(const CollisionPair& pair, std::vector<CollisionManifold>& manifolds);

	private:
		void(*collisionFunctions[3])(const CollisionPair& pair,
			std::vector<CollisionManifold>& manifolds) = {
			DoCircleVsCircle,
			DoPolygonVsCircle,
			DoPolygonVsPolygon
		};
	};
}
