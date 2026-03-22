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
			RecalculatePoints();
		}

		void SetCenter(const glm::vec2 &center)
		{
			if (glm::length(center - m_center) > 0.0f)
			{
				m_center = center;
				RecalculatePoints();
			}
		}

		void SetHalfExtents(const glm::vec2 &&halfExtents)
		{
			if (glm::length(halfExtents - m_halfExtents) > 0.001f)
			{
				m_halfExtents = halfExtents;
				RecalculatePoints();
			}
		}

		void SetRotation(const float radians)
		{
			m_rotation = radians;
			RecalculatePoints();
		}

	private:
		void RecalculatePoints()
		{
			TranslatePoints();
			RotatePoints();
		}

		void TranslatePoints()
		{
			m_topLeft = glm::vec2(m_center.x - m_halfExtents.x, m_center.y + m_halfExtents.y);
			m_topRight = glm::vec2(m_center.x + m_halfExtents.x, m_center.y + m_halfExtents.y);
			m_bottomLeft = glm::vec2(m_center.x - m_halfExtents.x, m_center.y - m_halfExtents.y);
			m_bottomRight = glm::vec2(m_center.x + m_halfExtents.x, m_center.y - m_halfExtents.y);
		}

		void RotatePoints()
		{
			Core::MathHelpers::RotatePointAroundCenter(m_topLeft, m_center, m_rotation);
			Core::MathHelpers::RotatePointAroundCenter(m_topRight, m_center, m_rotation);
			Core::MathHelpers::RotatePointAroundCenter(m_bottomLeft, m_center, m_rotation);
			Core::MathHelpers::RotatePointAroundCenter(m_bottomRight, m_center, m_rotation);
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
