#pragma once
#include <cmath>
#include "vec2.hpp"

namespace Core::MathHelpers
{
	static inline void RotatePointAroundCenter(
		glm::vec2& point,
		const glm::vec2& center,
		const float radians)
	{
		//translate the point back to origin, i.e. what would it's position be the box's center was the origin?
		point -= center;

		float cosine = std::cos(radians);
		float sine = std::sin(radians);

		//Rotate the point in the origin's coordinate system, and then translate it back to the original position,
		//i.e. centered around the actual box's center
		point = glm::vec2(point.x * cosine - point.y * sine, point.x * sine + point.y * cosine) + center;
	}
}
