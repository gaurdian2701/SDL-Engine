#pragma once
#include <vector>

namespace Components
{
	struct CircleCollider2D;
	struct PolygonCollider2D;
	struct Transform;
	struct Rigidbody2D;
}

namespace Core::Physics
{
	namespace PhysicsData
	{
		struct CollisionPair;
		struct ContactManifold;
	}

	class NarrowPhase
	{
	public:
		NarrowPhase() = default;
		~NarrowPhase() = default;

		void GenerateManifolds(const std::vector<PhysicsData::CollisionPair>& collisionPairs,
			std::vector<PhysicsData::ContactManifold>& manifolds);

	private:
		static void DoCircleVsCircle(const PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds);
		static void DoPolygonVsCircle(const PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds);
		static void DoPolygonVsPolygon(const PhysicsData::CollisionPair& pair, std::vector<PhysicsData::ContactManifold>& manifolds);

	private:
		void(*collisionFunctions[3])(const PhysicsData::CollisionPair& pair,
			std::vector<PhysicsData::ContactManifold>& manifolds) = {
			DoCircleVsCircle,
			DoPolygonVsCircle,
			DoPolygonVsPolygon
		};
	};
}
