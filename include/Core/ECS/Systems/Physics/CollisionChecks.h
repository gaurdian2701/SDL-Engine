#pragma once
#include "Core/HelperFunctions.h"

namespace CollisionChecks
{
	static inline bool AABBVsAABB(const Components::AABB& aabb1, const Components::AABB& aabb2)
	{
		if (aabb1.MaxPoint.x < aabb2.MinPoint.x || aabb1.MinPoint.x > aabb2.MaxPoint.x)
		{
			return false;
		}

		if (aabb1.MaxPoint.y < aabb2.MinPoint.y || aabb1.MinPoint.y > aabb2.MaxPoint.y)
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

	static inline void ProjectPointsOnAxis(const std::vector<glm::vec2>& points,
		const glm::vec2& axis,
		float& maxProjectionValueOnAxis,
		float& minProjectionValueOnAxis)
	{
		float max = FLT_MIN;
		float min = FLT_MAX;

		for (uint32_t pointIndex = 0; pointIndex < points.size(); pointIndex++)
		{
			//"Project" the point on the axis using dot product
			float projectionOnAxis = glm::dot(points[pointIndex], axis);
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

	static inline void ProjectCircleOnAxis(const glm::vec2& center,
		float radius,
		const glm::vec2& axis,
		float& maxProjectionValueOnAxis,
		float& minProjectionValueOnAxis)
	{
		//A vector signifying the direction of the separation axis along with the magnitude of the radius
		glm::vec2 directionAndRadiusVector = glm::normalize(axis) * radius;

		//We get the two edge points of the circle
		glm::vec2 rightPoint = center + directionAndRadiusVector;
		glm::vec2 leftPoint = center - directionAndRadiusVector;

		maxProjectionValueOnAxis = glm::dot(rightPoint, axis);
		minProjectionValueOnAxis = glm::dot(leftPoint, axis);

		//But max might not actually be the max value and min might not actually be the min value in some cases,
		//so we swap them in those cases
		if (maxProjectionValueOnAxis < minProjectionValueOnAxis)
		{
			std::swap(maxProjectionValueOnAxis, minProjectionValueOnAxis);
		}
	}

	static inline bool PolygonVsPolygon(const glm::vec2& polygonCenterA,
		const glm::vec2& polygonCenterB,
		const std::vector<glm::vec2>& polygonPointsA,
		const std::vector<glm::vec2>& polygonPointsB,
		float& penetrationDepth,
		glm::vec2& contactNormal)
	{
		float minSeparationDepth = FLT_MAX;
		glm::vec2 resolutionNormal = glm::vec2(0.0f); //Normal used for resolving collisions

		//Separating Axis Theorem for detecting collisions between polygons
		for (uint32_t pointIndex = 0; pointIndex < polygonPointsA.size(); pointIndex++)
		{
			//Find normal of edge between first and next point
			const glm::vec2& firstPoint = polygonPointsA[pointIndex];
			const glm::vec2& secondPoint = polygonPointsA[(pointIndex+1)%polygonPointsA.size()];
			const glm::vec2 edge = secondPoint - firstPoint;

			glm::vec2 separatingAxis = glm::vec2(-edge.y, edge.x);
			separatingAxis = glm::normalize(separatingAxis);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			float maxAOnAxis = 0.0f, minAOnAxis = 0.0f, maxBOnAxis = 0.0f, minBOnAxis = 0.0f;
			ProjectPointsOnAxis(polygonPointsA, separatingAxis, maxAOnAxis, minAOnAxis);
			ProjectPointsOnAxis(polygonPointsB, separatingAxis, maxBOnAxis, minBOnAxis);

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
				resolutionNormal = separatingAxis;
			}
		}

		//Repeat for points on B
		for (uint32_t pointIndex = 0; pointIndex < polygonPointsB.size(); pointIndex++)
		{
			//Find normal of edge between first and next point
			const glm::vec2& firstPoint = polygonPointsB[pointIndex];
			const glm::vec2& secondPoint = polygonPointsB[(pointIndex+1)%polygonPointsB.size()];
			const glm::vec2 edge = secondPoint - firstPoint;
			glm::vec2 axisNormal = glm::vec2(-edge.y, edge.x);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			float maxAOnAxis = 0.0f, minAOnAxis = 0.0f, maxBOnAxis = 0.0f, minBOnAxis = 0.0f;
			ProjectPointsOnAxis(polygonPointsA, axisNormal, maxAOnAxis, minAOnAxis);
			ProjectPointsOnAxis(polygonPointsB, axisNormal, maxBOnAxis, minBOnAxis);

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
		glm::vec2 centerScreenA = Core::WorldToScreen(polygonCenterA);
		glm::vec2 centerScreenB = Core::WorldToScreen(polygonCenterB);
		glm::vec2 screenAEnd = Core::WorldToScreen(polygonCenterA + minSeparationDepth * glm::normalize(resolutionNormal));
		glm::vec2 screenBEnd = Core::WorldToScreen(polygonCenterB - minSeparationDepth * glm::normalize(resolutionNormal));

		SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), 255, 0, 0, 255);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), centerScreenA.x, centerScreenA.y, screenAEnd.x, screenAEnd.y);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), centerScreenB.x, centerScreenB.y, screenBEnd.x, screenBEnd.y);
#endif

		penetrationDepth = minSeparationDepth;
		contactNormal = glm::normalize(resolutionNormal);

		glm::vec2 directionVector = polygonCenterB - polygonCenterA;
		if (glm::dot(contactNormal, directionVector) < 0.0f)
		{
			contactNormal = -contactNormal;
		}
		return true;
	}

	static inline bool PolygonVsCircle(const glm::vec2& polygonCenter,
		const std::vector<glm::vec2>& polygonPoints,
		const glm::vec2& circleCenter,
		float circleRadius,
		float& penetrationDepth,
		glm::vec2& contactNormal)
	{
		float minSeparationDepth = FLT_MAX;
		float axisDepth = 0.0f;
		float maxPolygonPointOnAxis = 0.0f, minPolygonPointOnAxis = 0.0f, maxCirclePointOnAxis = 0.0f, minCirclePointOnAxis = 0.0f;

		glm::vec2 separatingAxis = glm::vec2(0.0f);
		glm::vec2 resolutionNormal = glm::vec2(0.0f); //Normal used for resolving collisions

		//Also uses SAT for polygon vs circle collisions
		for (uint32_t pointIndex = 0; pointIndex < polygonPoints.size(); pointIndex++)
		{
			//Find normal of edge between first and next point
			const glm::vec2& firstPoint = polygonPoints[pointIndex];
			const glm::vec2& secondPoint = polygonPoints[(pointIndex+1)%polygonPoints.size()];
			const glm::vec2 edge = secondPoint - firstPoint;

			//The normal of the edge is taken as the separating axis
			separatingAxis = glm::vec2(-edge.y, edge.x);
			separatingAxis = glm::normalize(separatingAxis);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			ProjectPointsOnAxis(polygonPoints, separatingAxis, maxPolygonPointOnAxis, minPolygonPointOnAxis);
			ProjectCircleOnAxis(circleCenter, circleRadius, separatingAxis, maxCirclePointOnAxis, minCirclePointOnAxis);

			//Using max and min projections, if a gap is found, return false
			if (minPolygonPointOnAxis >= maxCirclePointOnAxis || maxPolygonPointOnAxis <= minCirclePointOnAxis)
			{
				return false;
			}

			//Find the magnitude of separation
			axisDepth = std::min(glm::length(maxCirclePointOnAxis - minPolygonPointOnAxis),
				glm::length(maxPolygonPointOnAxis - minCirclePointOnAxis));
			if (axisDepth < minSeparationDepth)
			{
				minSeparationDepth = axisDepth;
				resolutionNormal = separatingAxis;
			}
		}

		float minDistance = FLT_MAX;
		uint32_t closestPointIndex = 0;

		//Now to check for collision from the circle's end, first find the point on the polygon that is closest to the circle.
		for (uint32_t pointIndex = 0; pointIndex < polygonPoints.size(); pointIndex++)
		{
			const glm::vec2& point = polygonPoints[pointIndex];
			float pointDistance = glm::distance(point, circleCenter);
			if (pointDistance < minDistance)
			{
				minDistance = pointDistance;
				closestPointIndex = pointIndex;
			}
			++pointIndex;
		}

		//Then, the vector from the center to the closest point becomes the separating axis
		separatingAxis = circleCenter - polygonPoints[closestPointIndex];
		separatingAxis = glm::normalize(separatingAxis);

		//Project again on new axis
		ProjectPointsOnAxis(polygonPoints, separatingAxis, maxPolygonPointOnAxis, minPolygonPointOnAxis);
		ProjectCircleOnAxis(circleCenter, circleRadius, separatingAxis, maxCirclePointOnAxis, minCirclePointOnAxis);

		//Using max and min projections, if a gap is found, return false
		if (minPolygonPointOnAxis >= maxCirclePointOnAxis || maxPolygonPointOnAxis <= minCirclePointOnAxis)
		{
			return false;
		}

		//Find the magnitude of separation
		axisDepth = std::min(glm::length(maxCirclePointOnAxis - minPolygonPointOnAxis), glm::length(maxPolygonPointOnAxis - minCirclePointOnAxis));
		if (axisDepth < minSeparationDepth)
		{
			minSeparationDepth = axisDepth;
			resolutionNormal = separatingAxis;
		}

		//Since we were computing the depth with un-normalized axisNormal, we divide by the length of the normal
		//here to get the proper depth as if it was computed with the normalized axis normal
		minSeparationDepth /= glm::length(resolutionNormal);

#ifdef _DEBUG
		glm::vec2 polygonCenterInScreenCoords = Core::WorldToScreen(polygonCenter);
		glm::vec2 polygonEndPointInScreenCoords = Core::WorldToScreen(polygonCenter + minSeparationDepth * glm::normalize(resolutionNormal));
		glm::vec2 circleCenterInScreenCoords = Core::WorldToScreen(circleCenter);
		glm::vec2 circleEndPointInScreenCoords = Core::WorldToScreen(circleCenter - minSeparationDepth * glm::normalize(resolutionNormal));

		SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), 255, 0, 0, 255);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), polygonCenterInScreenCoords.x, polygonCenterInScreenCoords.y, polygonEndPointInScreenCoords.x, polygonEndPointInScreenCoords.y);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), circleCenterInScreenCoords.x, circleCenterInScreenCoords.y, circleEndPointInScreenCoords.x, circleEndPointInScreenCoords.y);
#endif

		penetrationDepth = minSeparationDepth;
		contactNormal = glm::normalize(resolutionNormal);

		glm::vec2 directionVector = circleCenter - polygonCenter;
		if (glm::dot(contactNormal, directionVector) < 0.0f)
		{
			contactNormal = -contactNormal;
		}
		return true;
	}
}