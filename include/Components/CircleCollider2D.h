#pragma once
#include "AABB.h"
#include "vec2.hpp"
#include "geometric.hpp"

namespace Components
{
	struct CircleCollider2D
	{
		CircleCollider2D()
		{
			aabb.Collider = ColliderType::CIRCLE;
		}

		~CircleCollider2D() = default;

		void Initialize(const glm::vec2& someCenter, float someRadius)
		{
			center = someCenter;
			radius = someRadius;
			UpdateAABB();
		}

		void SetPosition(const glm::vec2& newPos)
		{
			if (glm::length(newPos - center) > 0.1f)
			{
				center = newPos;
				UpdateAABB();
			}
		}

		void SetRadius(const float newRadius)
		{
			if (newRadius > 0.0f)
			{
				if (std::abs(newRadius - radius) > 0.1f)
				{
					radius = newRadius;
					UpdateAABB();
				}
			}
		}

		AABB& GetAABB()
		{
			return aabb;
		}

		float GetRadius() const
		{
			return radius;
		}

		const glm::vec2& GetCenter() const
		{
			return center;
		}


		float CalculateMomentOfInertia(float mass) const
		{
			return mass * radius * radius * 0.5f;
		}

	private:
		void UpdateAABB()
		{
			aabb.BottomLeft = center - radius;
			aabb.TopRight = center + radius;
		}

	public:
		bool IsColliding = false;
		bool MatchScaleWithTransform = true;

	private:
		AABB aabb = AABB();
		glm::vec2 center = glm::vec2(0.0f);
		float radius = 1.0f;
	};
}