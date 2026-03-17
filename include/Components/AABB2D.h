#pragma once
#include "vec2.hpp"

namespace Components
{
	struct AABB2D
	{
		AABB2D() = default;
		~AABB2D() = default;

		//Copy Constructor
		AABB2D(const AABB2D& other)
		{
			HalfExtents = other.HalfExtents;
		}

		//Move Constructor
		AABB2D(AABB2D&& other) noexcept
		{
			if (this != &other)
			{
				HalfExtents = other.HalfExtents;
			}
		}

		//Copy Assignment Operator
		AABB2D& operator=(const AABB2D& other)
		{
			if (this != &other)
			{
				HalfExtents = other.HalfExtents;
			}
			return *this;
		}

		//Move Assignment Operator
		AABB2D& operator=(AABB2D&& other) noexcept
		{
			if (this != &other)
			{
				HalfExtents = other.HalfExtents;
			}
			return *this;
		}

		glm::vec2 HalfExtents = glm::vec2(0.0f);
	};
}
