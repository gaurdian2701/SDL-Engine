#pragma once
#include <cmath>
#include "vec2.hpp"
#include "ext/quaternion_geometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "gtx/norm.hpp"

namespace Core::MathHelpers
{
	static void RotatePointAroundCenter(
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

	template<typename T>
	static int GetSign(T number)
	{
		if (number < 0) return -1;
		return 1;
	}

	static glm::vec2 GetMidPoint(glm::vec2 a, glm::vec2 b)
	{
		return {(a.x + b.x) * 0.5f, (a.y + b.y) * 0.5f};
	}

	static float PointLineSegmentDistance(const glm::vec2& pointA,
	const glm::vec2& pointB,
	const glm::vec2& referencePoint,
	glm::vec2& closestPoint)
	{
		glm::vec2 ab = pointB - pointA;
		glm::vec2 ap = referencePoint - pointA;

		float projection = glm::dot(ap, ab);
		float magnitudeAlongLine = projection / glm::length2(ab);

		if (magnitudeAlongLine <= 0.0f)
		{
			closestPoint = pointA;
		}
		else if (magnitudeAlongLine >= 1.0f)
		{
			closestPoint = pointB;
		}
		else
		{
			closestPoint = pointA + ab * magnitudeAlongLine;
		}

		return glm::distance(referencePoint, closestPoint);
	}
}
