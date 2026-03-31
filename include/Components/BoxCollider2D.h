#pragma once
#include "Core/MathHelpers.h"
#include "vec2.hpp"
#include "geometric.hpp"
#include "PrintDebug.h"

namespace Components
{
	struct BoxCollider2D
	{
		BoxCollider2D() = default;
		~BoxCollider2D() = default;

		const glm::vec2& GetCenter() const
		{
			return center;
		}

		float GetRotation() const
		{
			return rotation;
		}

		const glm::vec2 &GetHalfExtents() const
		{
			return halfExtents;
		}

		const glm::vec2 &GetTopLeftPoint() const
		{
			return topLeft;
		}

		const glm::vec2 &GetTopRightPoint() const
		{
			return topRight;
		}

		const glm::vec2 &GetBottomLeftPoint() const
		{
			return bottomLeft;
		}

		const glm::vec2 &GetBottomRightPoint() const
		{
			return bottomRight;
		}

		void Initialize(const glm::vec2 &someCenter,
			const glm::vec2&& someHalfExtents,
			float someRotationInRadians)
		{
			center = someCenter;
			halfExtents = someHalfExtents;
			rotation = someRotationInRadians;
			RecalculateBounds();
		}

		void UpdatePositionAndRotation(const glm::vec2 &someCenter, const float someRotationInRadians)
		{
			if (glm::length(someCenter - center) > 0.0f)
			{
				TranslateBounds(someCenter);
				center = someCenter;
			}

			if (glm::abs(someRotationInRadians - rotation) > 0.0f)
			{
				RotateBounds(someRotationInRadians - rotation);
				rotation = someRotationInRadians;
			}
		}

		void SetHalfExtents(const glm::vec2 &&someHalfExtents)
		{
			if (glm::length(someHalfExtents - halfExtents) > 0.0f)
			{
				halfExtents = someHalfExtents;
				ReInitializeCollider();
			}
		}

	private:
		void ReInitializeCollider()
		{
			RecalculateBounds();
			RotateBounds(rotation);
		}

		void TranslateBounds(const glm::vec2& toPoint)
		{
			glm::vec2 translationVector = toPoint - center;

			topLeft += translationVector;
			topRight += translationVector;
			bottomLeft += translationVector;
			bottomRight += translationVector;
		}

		void RecalculateBounds()
		{
			topLeft = glm::vec2(center.x - halfExtents.x, center.y + halfExtents.y);
			topRight = glm::vec2(center.x + halfExtents.x, center.y + halfExtents.y);
			bottomLeft = glm::vec2(center.x - halfExtents.x, center.y - halfExtents.y);
			bottomRight = glm::vec2(center.x + halfExtents.x, center.y - halfExtents.y);
		}

		void RotateBounds(const float radians)
		{
			Core::MathHelpers::RotatePointAroundCenter(topLeft, center, radians);
			Core::MathHelpers::RotatePointAroundCenter(topRight, center, radians);
			Core::MathHelpers::RotatePointAroundCenter(bottomLeft, center, radians);
			Core::MathHelpers::RotatePointAroundCenter(bottomRight, center, radians);
		}

		glm::vec2 center = glm::vec2(0.0f);
		glm::vec2 halfExtents = glm::vec2(0.0f);
		glm::vec2 topLeft = glm::vec2(0.0f);
		glm::vec2 topRight = glm::vec2(0.0f);
		glm::vec2 bottomLeft = glm::vec2(0.0f);
		glm::vec2 bottomRight = glm::vec2(0.0f);
		float rotation = 0.0f;

	public:
		bool IsColliding = false;
	};
}
