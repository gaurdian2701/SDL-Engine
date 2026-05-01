#pragma once
#include "AABB.h"
#include "Core/MathHelpers.h"
#include "vec2.hpp"
#include "geometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/exterior_product.hpp"

namespace Components
{
	// Uses Counter-Clockwise winding order for storing points
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

		const std::vector<glm::vec2>& GetVertices() const
		{
			return points;
		}

		void InitializePolygon(const glm::vec2& someCenter,
			int numberOfVertices,
			float someScale,
			float someRotationInRadians)
		{
			center = someCenter;
			rotation = someRotationInRadians;
			points = std::vector<glm::vec2>(numberOfVertices);

			Core::MathHelpers::GeneratePolygonPointsAroundCircle(
				center,
				someScale,
				points,
				numberOfVertices);

			RotateBounds(rotation);
			UpdateAABB();
		}

		void InitializeBox(const glm::vec2& someCenter,
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

		float CalculateMomentOfInertia(float mass) const
		{
			float area = 0.0f;
			float mmoi = 0.0f; //mass moment of inertia
			glm::vec2 center = glm::vec2(0.0f);

			std::uint32_t prevIndex = points.size()-1;

			for (std::uint32_t currentIndex = 0; currentIndex < points.size(); currentIndex++)
			{
				glm::vec2 currentPoint = points[currentIndex];
				glm::vec2 previousPoint = points[prevIndex];

				float areaStep = glm::cross(currentPoint, previousPoint) * 0.5f;
				glm::vec2 centerStep = (currentPoint + previousPoint) / 3.0f;
				float mmoiStep = areaStep * (glm::dot(currentPoint, currentPoint) +
					glm::dot(previousPoint, previousPoint) +
					glm::dot(currentPoint, previousPoint)) / 6.0f;

				center = (center * area + centerStep * areaStep) / (area + areaStep);
				area += areaStep;
				mmoi += mmoiStep;

				prevIndex = currentIndex;
			}

			float density = mass/area;
			mmoi *= density;
			mmoi -= mass * glm::dot(center, center);

			return mmoi;
		}

		void ScaleBounds(float someScale)
		{
			for (auto& point: points)
			{
				point *= someScale;
			}
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
				points[0] = glm::vec2(center.x - halfExtents.x, center.y - halfExtents.y); //bottom left
				points[1] = glm::vec2(center.x + halfExtents.x, center.y - halfExtents.y); //bottom right
				points[2] = glm::vec2(center.x + halfExtents.x, center.y + halfExtents.y); //top right
				points[3] = glm::vec2(center.x - halfExtents.x, center.y + halfExtents.y); //top left
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
		bool MatchScaleWithTransform = true;
	};
}
