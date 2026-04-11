#pragma once
#include "AABB.h"
#include "vec2.hpp"
#include "geometric.hpp"

namespace Components
{
	struct CircleCollider2D
	{
		CircleCollider2D() = default;
		explicit CircleCollider2D(float someRadius)
		{
			Radius = someRadius;
		}
		~CircleCollider2D() = default;

		void Initialize(const glm::vec2& someCenter, float someRadius)
		{
			Center = someCenter;
			Radius = someRadius;
			aabb.MinPoint = Center - Radius;
			aabb.MaxPoint = Center + Radius;
		}

		void UpdatePosition(const glm::vec2& newPos)
		{
			if (glm::length(newPos - Center) > 0.1f)
			{
				Center = newPos;
			}
		}

		const AABB& GetAABB() const
		{
			return aabb;
		}

		glm::vec2 Center = glm::vec2(0.0f);
		float Radius = 1.0f;
		bool IsColliding = false;

	private:
		AABB aabb = AABB();
	};
}