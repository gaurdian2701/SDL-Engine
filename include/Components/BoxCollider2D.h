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
			return m_center;
		}

		float GetRotation() const
		{
			return m_rotation;
		}

		const glm::vec2 &GetHalfExtents() const
		{
			return m_halfExtents;
		}

		const glm::vec2 &GetTopLeftPoint() const
		{
			return m_topLeft;
		}

		const glm::vec2 &GetTopRightPoint() const
		{
			return m_topRight;
		}

		const glm::vec2 &GetBottomLeftPoint() const
		{
			return m_bottomLeft;
		}

		const glm::vec2 &GetBottomRightPoint() const
		{
			return m_bottomRight;
		}

		void Initialize(const glm::vec2 &center,
			const glm::vec2&& halfExtents,
			float rotationInRadians)
		{
			m_center = center;
			m_halfExtents = halfExtents;
			m_rotation = rotationInRadians;
			RecalculateBounds();
		}

		void UpdatePositionAndRotation(const glm::vec2 &center, const float rotationInRadians)
		{
			if (glm::length(center - m_center) > 0.0f)
			{
				TranslateBounds(center);
				m_center = center;
			}

			if (glm::abs(rotationInRadians - m_rotation) > 0.0f)
			{
				RotateBounds(rotationInRadians - m_rotation);
				m_rotation = rotationInRadians;
			}
		}

		void SetHalfExtents(const glm::vec2 &&halfExtents)
		{
			if (glm::length(halfExtents - m_halfExtents) > 0.0f)
			{
				m_halfExtents = halfExtents;
				ReInitializeCollider();
			}
		}

	private:
		void ReInitializeCollider()
		{
			RecalculateBounds();
			RotateBounds(m_rotation);
		}

		void TranslateBounds(const glm::vec2& toPoint)
		{
			glm::vec2 translationVector = toPoint - m_center;

			m_topLeft += translationVector;
			m_topRight += translationVector;
			m_bottomLeft += translationVector;
			m_bottomRight += translationVector;
		}

		void RecalculateBounds()
		{
			m_topLeft = glm::vec2(m_center.x - m_halfExtents.x, m_center.y + m_halfExtents.y);
			m_topRight = glm::vec2(m_center.x + m_halfExtents.x, m_center.y + m_halfExtents.y);
			m_bottomLeft = glm::vec2(m_center.x - m_halfExtents.x, m_center.y - m_halfExtents.y);
			m_bottomRight = glm::vec2(m_center.x + m_halfExtents.x, m_center.y - m_halfExtents.y);
		}

		void RotateBounds(const float radians)
		{
			Core::MathHelpers::RotatePointAroundCenter(m_topLeft, m_center, radians);
			Core::MathHelpers::RotatePointAroundCenter(m_topRight, m_center, radians);
			Core::MathHelpers::RotatePointAroundCenter(m_bottomLeft, m_center, radians);
			Core::MathHelpers::RotatePointAroundCenter(m_bottomRight, m_center, radians);
		}

		glm::vec2 m_center = glm::vec2(0.0f);
		glm::vec2 m_halfExtents = glm::vec2(0.0f);
		glm::vec2 m_topLeft = glm::vec2(0.0f);
		glm::vec2 m_topRight = glm::vec2(0.0f);
		glm::vec2 m_bottomLeft = glm::vec2(0.0f);
		glm::vec2 m_bottomRight = glm::vec2(0.0f);
		float m_rotation = 0.0f;

	public:
		bool IsColliding = false;
	};
}
