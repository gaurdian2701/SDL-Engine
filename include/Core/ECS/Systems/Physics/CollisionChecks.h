#pragma once
#include "Core/HelperFunctions.h"

namespace CollisionChecks
{
	static inline bool AABBVsAABB(const glm::vec2& pos1,
	const glm::vec2& pos2,
	const glm::vec2& halfExtents1,
	const glm::vec2& halfExtents2)
	{
		//Using SAT(Separating Axis Theorem)
		//box1's maxX is lesser than box2's minX?
		if (pos1.x + halfExtents1.x < pos2.x - halfExtents2.x ||
			pos1.x - halfExtents1.x > pos2.x + halfExtents2.x)
		{
			return false;
		}
		//box1's maxY is lesser than box2's minY?
		if (pos1.y + halfExtents1.y < pos2.y - halfExtents2.y ||
			pos1.y - halfExtents1.y > pos2.y + halfExtents2.y)
		{
			return false;
		}

		return true;
	}

	static inline bool CircleVsCircle(const glm::vec2& center1,
		const glm::vec2& center2,
		float radiusSum)
	{
		radiusSum *= radiusSum;
		return radiusSum > (center1.x - center2.x) * (center1.x - center2.x) +
			(center1.y - center2.y) * (center1.y - center2.y);
	}

	void ProjectPointsOnAxis(const std::vector<const glm::vec2*>& points,
		const glm::vec2& axis,
		float& maxProjectionValueOnAxis,
		float& minProjectionValueOnAxis)
	{
		float max = FLT_MIN;
		float min = FLT_MAX;

		for (uint32_t pointIndex = 0; pointIndex < points.size(); pointIndex++)
		{
			float projectionOnAxis = glm::dot(*points[pointIndex], axis);
			if (projectionOnAxis > max)
			{
				max = projectionOnAxis;
			}
			if (projectionOnAxis < min)
			{
				min = projectionOnAxis;
			}
		}

		maxProjectionValueOnAxis = max;
		minProjectionValueOnAxis = min;
	}

	static inline bool PolygonVsPolygon(const glm::vec2& centerA,
		const glm::vec2& centerB,
		const std::vector<const glm::vec2*>& pointsA,
		const std::vector<const glm::vec2*>& pointsB,
		float& penetrationDepth,
		glm::vec2& contactNormal)
	{
		float minSeparationDepth = FLT_MAX;
		glm::vec2 resolutionNormal = glm::vec2(0.0f); //Normal used for resolving collisions

		//Separating Axis Theorem for detecting collisions between polygons
		for (uint32_t pointIndex = 0; pointIndex < pointsA.size(); pointIndex++)
		{
			//Find normal of edge between first and next point
			const glm::vec2& firstPoint = *pointsA[pointIndex];
			const glm::vec2& secondPoint = *pointsA[(pointIndex+1)%pointsA.size()];
			const glm::vec2 edge = secondPoint - firstPoint;

			glm::vec2 axisNormal = glm::vec2(-edge.y, edge.x);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			float maxAOnAxis = 0.0f, minAOnAxis = 0.0f, maxBOnAxis = 0.0f, minBOnAxis = 0.0f;
			ProjectPointsOnAxis(pointsA, axisNormal, maxAOnAxis, minAOnAxis);
			ProjectPointsOnAxis(pointsB, axisNormal, maxBOnAxis, minBOnAxis);

			//Using max and min projections, if a gap is found, return false
			if (minAOnAxis >= maxBOnAxis || maxAOnAxis <= minBOnAxis)
			{
				return false;
			}

			//Find the magnitude of separation
			float axisDepth = std::min(glm::length(maxBOnAxis - minAOnAxis), glm::length(maxAOnAxis - minBOnAxis));
			if (axisDepth < minSeparationDepth)
			{
				minSeparationDepth = axisDepth;
				resolutionNormal = axisNormal;
			}
		}

		//Repeat for points on B
		for (uint32_t pointIndex = 0; pointIndex < pointsB.size(); pointIndex++)
		{
			//Find normal of edge between first and next point
			const glm::vec2& firstPoint = *pointsB[pointIndex];
			const glm::vec2& secondPoint = *pointsB[(pointIndex+1)%pointsB.size()];
			const glm::vec2 edge = secondPoint - firstPoint;
			glm::vec2 axisNormal = glm::vec2(-edge.y, edge.x);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			float maxAOnAxis = 0.0f, minAOnAxis = 0.0f, maxBOnAxis = 0.0f, minBOnAxis = 0.0f;
			ProjectPointsOnAxis(pointsA, axisNormal, maxAOnAxis, minAOnAxis);
			ProjectPointsOnAxis(pointsB, axisNormal, maxBOnAxis, minBOnAxis);

			//Using max and min projections, if a gap is found, return false
			if (minAOnAxis >= maxBOnAxis || maxAOnAxis <= minBOnAxis)
			{
				return false;
			}

			//Find the magnitude of separation
			float axisDepth = std::min(glm::length(maxBOnAxis - minAOnAxis), glm::length(maxAOnAxis - minBOnAxis));
			if (axisDepth < minSeparationDepth)
			{
				minSeparationDepth = axisDepth;
				resolutionNormal = axisNormal;
			}
		}

		//Since we were computing the depth with un-normalized axisNormal, we divide by the length of the normal
		//here to get the proper depth as if it was computed with the normalized axis normal
		minSeparationDepth /= glm::length(resolutionNormal);

#ifdef _DEBUG
		glm::vec2 centerScreenA = Core::WorldToScreen(centerA);
		glm::vec2 centerScreenB = Core::WorldToScreen(centerB);
		glm::vec2 screenAEnd = Core::WorldToScreen(centerA + minSeparationDepth * glm::normalize(resolutionNormal));
		glm::vec2 screenBEnd = Core::WorldToScreen(centerB - minSeparationDepth * glm::normalize(resolutionNormal));

		SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), 255, 0, 0, 255);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), centerScreenA.x, centerScreenA.y, screenAEnd.x, screenAEnd.y);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), centerScreenB.x, centerScreenB.y, screenBEnd.x, screenBEnd.y);
#endif

		penetrationDepth = minSeparationDepth;
		contactNormal = glm::normalize(resolutionNormal);

		return true;
	}
}
