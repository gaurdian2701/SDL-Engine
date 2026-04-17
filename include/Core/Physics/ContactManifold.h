#pragma once
#include "vec2.hpp"

namespace Components
{
	struct Rigidbody2D;
	struct Transform;
}

namespace Core::Physics::PhysicsData
{
	struct ContactManifold
	{
		ContactManifold() = default;

		ContactManifold(Components::Transform* someTransformA,
			Components::Transform* someTransformB,
			Components::Rigidbody2D* someRigidbodyA,
			Components::Rigidbody2D* someRigidbodyB,
			glm::vec2&& someContactNormal,
			float somePenetrationDepth) :
			TransformA(someTransformA),
			TransformB(someTransformB),
			RigidbodyA(someRigidbodyA),
			RigidbodyB(someRigidbodyB),
			ContactNormal(someContactNormal),
			PenetrationDepth(somePenetrationDepth)
		{

		}

		~ContactManifold() = default;

		Components::Transform* TransformA = nullptr;
		Components::Transform* TransformB = nullptr;
		Components::Rigidbody2D* RigidbodyA = nullptr;
		Components::Rigidbody2D* RigidbodyB = nullptr;

		glm::vec2 ContactNormal = glm::vec2(0);
		float PenetrationDepth = 0.0f;
		float AccumulatedImpulse = 0.0f;
	};
}
