#pragma once
#include "vec2.hpp"

namespace Components
{
	struct AABB2D
	{
		AABB2D() = default;
		explicit AABB2D(glm::vec2 someHalfExtents)
		{
			HalfExtents = someHalfExtents;
		}
		~AABB2D() = default;

		glm::vec2 HalfExtents = glm::vec2(0.0f);
		bool IsColliding = false;
	};

}
