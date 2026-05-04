#pragma once
#include "vec2.hpp"
#include "Core/ECS/ECSData.h"

namespace Components
{
	enum class ColliderType : uint8_t
	{
		CIRCLE,
		POLYGON
	};

	struct AABB
	{
		AABB() = default;
		~AABB() = default;

		bool Overlaps(const AABB& other) const
		{
			if (TopRight.x < other.BottomLeft.x || BottomLeft.x > other.TopRight.x)
			{
				return false;
			}

			if (TopRight.y < other.BottomLeft.y || BottomLeft.y > other.TopRight.y)
			{
				return false;
			}

			return true;
		}

		bool Contains(const AABB& other) const
		{
			return other.BottomLeft.x >= BottomLeft.x &&
				other.BottomLeft.y >= BottomLeft.y &&
					other.TopRight.x <= TopRight.x &&
						other.TopRight.y <= TopRight.y;
		}

		glm::vec2 BottomLeft = glm::vec2(FLT_MAX);
		glm::vec2 TopRight = glm::vec2(FLT_MIN);
		ColliderType Collider = ColliderType::CIRCLE;
		std::uint32_t Entity = Core::INVALID_ENTITY_ID;
	};
}
