#pragma once
#include "vec2.hpp"

namespace Components
{
	struct AABB
	{
		AABB() = default;
		~AABB() = default;

		glm::vec2 MinPoint = glm::vec2(FLT_MAX);
		glm::vec2 MaxPoint = glm::vec2(FLT_MIN);
	};
}
