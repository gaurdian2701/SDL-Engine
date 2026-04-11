#pragma once
#include "AABB.h"
#include "Core/MathHelpers.h"
#include "vec2.hpp"
#include "geometric.hpp"

namespace Components
{
	struct PolygonCollider2D
	{
		PolygonCollider2D() = default;
		~PolygonCollider2D() = default;

		const glm::vec2& GetCenter() const
		{
			return center;
		}

		float GetRotation() const
		{
			return rotation;
		}

		const std::vector<glm::vec2>& GetPoints() const
		{
			return points;
		}

		void InitializeBox(const glm::vec2 &someCenter,
			const glm::vec2&& someHalfExtents,
			float someRotationInRadians)
		{
			center = someCenter;
			rotation = someRotationInRadians;
			points = std::vector<glm::vec2>(4);

			RecalculateBoxBounds(someHalfExtents);
			RotateBounds(rotation);
			UpdateAABB();
		}

		void UpdatePositionAndRotation(const glm::vec2 &someCenter, const float someRotationInRadians)
		{
			bool valuesChanged = false;
			if (glm::length(someCenter - center) > 0.0f)
			{
				TranslateBounds(someCenter);
				center = someCenter;
				valuesChanged = true;
			}

			if (glm::abs(someRotationInRadians - rotation) > 0.0f)
			{
				RotateBounds(someRotationInRadians - rotation);
				rotation = someRotationInRadians;
				valuesChanged = true;
			}

			if (valuesChanged)
			{
				UpdateAABB();
			}
		}

		void SetBoxHalfExtents(const glm::vec2 &&someHalfExtents)
		{
			RecalculateBoxBounds(someHalfExtents);
			RotateBounds(rotation);
			UpdateAABB();
		}

		const AABB& GetAABB() const
		{
			return aabb;
		}

	private:
		void TranslateBounds(const glm::vec2& toPoint)
		{
			glm::vec2 translationVector = toPoint - center;

			for (auto& point: points)
			{
				point += translationVector;
			}
		}

		void RecalculateBoxBounds(const glm::vec2& halfExtents)
		{
			if (points.size() == 4)
			{
				points[0] = glm::vec2(center.x - halfExtents.x, center.y + halfExtents.y); //top left
				points[1] = glm::vec2(center.x + halfExtents.x, center.y + halfExtents.y); //top right
				points[2] = glm::vec2(center.x + halfExtents.x, center.y - halfExtents.y); //bottom right
				points[3] = glm::vec2(center.x - halfExtents.x, center.y - halfExtents.y); //bottom left
			}
		}

		void RotateBounds(const float radians)
		{
			for (auto& point: points)
			{
				Core::MathHelpers::RotatePointAroundCenter(point, center, radians);
			}
		}

		void UpdateAABB()
		{
			glm::vec2 maxPoint = glm::vec2(std::numeric_limits<float>::lowest());
			glm::vec2 minPoint = glm::vec2(std::numeric_limits<float>::max());

			for (auto& point: points)
			{
				if (point.x > maxPoint.x)
				{
					maxPoint.x = point.x;
				}
				if (point.x < minPoint.x)
				{
					minPoint.x = point.x;
				}
				if (point.y > maxPoint.y)
				{
					maxPoint.y = point.y;
				}
				if (point.y < minPoint.y)
				{
					minPoint.y = point.y;
				}
			}

			aabb.MinPoint = minPoint;
			aabb.MaxPoint = maxPoint;
		}

		AABB aabb = AABB();
		glm::vec2 center = glm::vec2(0.0f);
		std::vector<glm::vec2> points = std::vector<glm::vec2>(3);
		float rotation = 0.0f;

	public:
		bool IsColliding = false;
	};
}
