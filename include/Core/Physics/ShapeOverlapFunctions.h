#pragma once
#include "Core/ScreenHelperFunctions.h"
#include "glm.hpp"
#include "Components/AABB.h"

namespace Core::Physics::ShapeOverlapFunctions
{
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
		float max = std::numeric_limits<float>::lowest();
		float min = std::numeric_limits<float>::max();

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

	static inline void FindReferenceEdge(const std::vector<glm::vec2>& polygonVerticesA,
		const std::vector<glm::vec2>& polygonVerticesB,
		const glm::vec2& contactNormal,
		glm::vec2& referenceEdge,
		std::size_t& referenceEdgeIndex,
		bool& aHasReferenceEdge)
	{
		//Find the reference edge for contact point generation - Its the edge which is
		//most perpendicular to the contact normal. In other words, it's the edge
		//whose normal is most aligned with the contact normal

		//Loop through all the normals of both polygons and find the edge which is most perpendicular with the contact normal
		const uint8_t numberOfVerticesA = polygonVerticesA.size();
		const uint8_t numberOfVerticesB = polygonVerticesB.size();

		uint8_t bestIndexA = 0;
		uint8_t bestIndexB = 0;

		float maxAlignmentA = std::numeric_limits<float>::lowest();
		float maxAlignmentB = std::numeric_limits<float>::lowest();

		float faceSwitchTolerance = 1e-4;

		//Start with A
		for (uint8_t i = 0; i < numberOfVerticesA; ++i)
		{
			const glm::vec2 currentEdge = polygonVerticesA[(i + 1) % numberOfVerticesA] - polygonVerticesA[i];
			glm::vec2 edgeNormal = glm::normalize(glm::vec2(currentEdge.y, -currentEdge.x));
			float currentAlignment = glm::dot(edgeNormal, contactNormal);
			if (currentAlignment > maxAlignmentA)
			{
				bestIndexA = i;
				maxAlignmentA = currentAlignment;
			}
		}

		//Then with B
		for (uint8_t i = 0; i < numberOfVerticesB; ++i)
		{
			const glm::vec2 currentEdge = polygonVerticesB[(i + 1) % numberOfVerticesB] - polygonVerticesB[i];
			glm::vec2 edgeNormal = glm::normalize(glm::vec2(currentEdge.y, -currentEdge.x));
			float currentAlignment = glm::dot(edgeNormal, -contactNormal);
			if (currentAlignment > maxAlignmentB)
			{
				bestIndexB = i;
				maxAlignmentB = currentAlignment;
			}
		}

		if (maxAlignmentA > maxAlignmentB + faceSwitchTolerance)
		{
			aHasReferenceEdge = true;
			referenceEdge = polygonVerticesA[(bestIndexA + 1) % numberOfVerticesA] - polygonVerticesA[bestIndexA];
			referenceEdgeIndex = bestIndexA;
		}
		else
		{
			aHasReferenceEdge = false;
			referenceEdge = polygonVerticesB[(bestIndexB + 1) % numberOfVerticesB] - polygonVerticesB[bestIndexB];
			referenceEdgeIndex = bestIndexB;
		}
	}

	static inline bool PolygonVsPolygon(const glm::vec2& polygonCenterA,
		const glm::vec2& polygonCenterB,
		const std::vector<glm::vec2>& polygonVerticesA,
		const std::vector<glm::vec2>& polygonVerticesB,
		float& penetrationDepth,
		glm::vec2& contactNormal)
	{
		float minPenetrationA = std::numeric_limits<float>::max();
		float minPenetrationB = std::numeric_limits<float>::max();

		glm::vec2 bestSeparatingAxisA = glm::vec2(0.0f);
		glm::vec2 bestSeparatingAxisB = glm::vec2(0.0f);

		std::size_t numberOfVerticesA = polygonVerticesA.size();
		std::size_t numberOfVerticesB = polygonVerticesB.size();

		//Separating Axis Theorem for detecting collisions between polygons
		//First start for points on polygon A
		for (uint8_t pointIndex = 0; pointIndex < numberOfVerticesA; pointIndex++)
		{
			//Find normal of edge between current and previous point
			const glm::vec2& currentPoint = polygonVerticesA[pointIndex];
			const glm::vec2& nextPoint = polygonVerticesA[(pointIndex + 1)%numberOfVerticesA];
			const glm::vec2 edge = nextPoint - currentPoint;

			glm::vec2 normalSeparatingAxis = glm::vec2(edge.y, -edge.x);
			normalSeparatingAxis = glm::normalize(normalSeparatingAxis);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			float maxAOnAxis = 0.0f, minAOnAxis = 0.0f, maxBOnAxis = 0.0f, minBOnAxis = 0.0f;
			ProjectPointsOnAxis(polygonVerticesA, normalSeparatingAxis, maxAOnAxis, minAOnAxis);
			ProjectPointsOnAxis(polygonVerticesB, normalSeparatingAxis, maxBOnAxis, minBOnAxis);

			//Using max and min projections, if a gap is found, return false
			if (minAOnAxis > maxBOnAxis || maxAOnAxis < minBOnAxis)
			{
				return false;
			}

			//Find min penetration
			float penetrationOnCurrentAxis = std::min(maxAOnAxis, maxBOnAxis) - std::max(minAOnAxis, minBOnAxis);
			if (penetrationOnCurrentAxis < minPenetrationA)
			{
				minPenetrationA = penetrationOnCurrentAxis;
				bestSeparatingAxisA = normalSeparatingAxis;
			}
		}

		//Repeat for points on B
		for (uint8_t pointIndex = 0; pointIndex < numberOfVerticesB; pointIndex++)
		{
			//Find normal of edge between current and previous point
			const glm::vec2& currentPoint = polygonVerticesB[pointIndex];
			const glm::vec2& nextPoint = polygonVerticesB[(pointIndex + 1)%numberOfVerticesB];
			const glm::vec2 edge = nextPoint - currentPoint;
			glm::vec2 normalSeparatingAxis = glm::vec2(edge.y, -edge.x);
			normalSeparatingAxis = glm::normalize(normalSeparatingAxis);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			float maxAOnAxis = 0.0f, minAOnAxis = 0.0f, maxBOnAxis = 0.0f, minBOnAxis = 0.0f;
			ProjectPointsOnAxis(polygonVerticesA, normalSeparatingAxis, maxAOnAxis, minAOnAxis);
			ProjectPointsOnAxis(polygonVerticesB, normalSeparatingAxis, maxBOnAxis, minBOnAxis);

			//Using max and min projections, if a gap is found, return false
			if (minAOnAxis > maxBOnAxis || maxAOnAxis < minBOnAxis)
			{
				return false;
			}

			//Find min penetration
			float penetrationOnCurrentAxis = std::min(maxAOnAxis, maxBOnAxis) - std::max(minAOnAxis, minBOnAxis);
			if (penetrationOnCurrentAxis < minPenetrationB)
			{
				minPenetrationB = penetrationOnCurrentAxis;
				bestSeparatingAxisB = normalSeparatingAxis;
			}
		}

		static float bias = 0.5f;

		//find contact normal and min penetration
		if (minPenetrationA < minPenetrationB - bias)
		{
			penetrationDepth = minPenetrationA;
			contactNormal = bestSeparatingAxisA;
		}
		else
		{
			penetrationDepth = minPenetrationB;
			contactNormal = bestSeparatingAxisB;
		}

DoDebug(
		glm::vec2 centerScreenA = Core::ScreenHelperFunctions::WorldToScreen(polygonCenterA);
		glm::vec2 centerScreenB = Core::ScreenHelperFunctions::WorldToScreen(polygonCenterB);
		glm::vec2 screenAEnd = Core::ScreenHelperFunctions::WorldToScreen(
			polygonCenterA + penetrationDepth * glm::normalize(contactNormal));
		glm::vec2 screenBEnd = Core::ScreenHelperFunctions::WorldToScreen(
			polygonCenterB - penetrationDepth * glm::normalize(contactNormal));

		SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), 255, 0, 0, 255);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), centerScreenA.x, centerScreenA.y, screenAEnd.x, screenAEnd.y);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), centerScreenB.x, centerScreenB.y, screenBEnd.x, screenBEnd.y);
);

		glm::vec2 directionVector = polygonCenterB - polygonCenterA;
		if (glm::dot(contactNormal, directionVector) < 0.0f)
		{
			contactNormal = -contactNormal;
		}
		return true;
	}

	static inline void ProjectCircleOnAxis(const glm::vec2& center,
		float radius,
		const glm::vec2& axis,
		float& maxProjectionValueOnAxis,
		float& minProjectionValueOnAxis)
	{
		//A vector signifying the direction of the separation axis along with the magnitude of the radius
		glm::vec2 directionAndRadiusVector = axis * radius;

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

	static inline bool PolygonVsCircle(const glm::vec2& polygonCenter,
		const std::vector<glm::vec2>& polygonPoints,
		const glm::vec2& circleCenter,
		float circleRadius,
		float& penetrationDepth,
		glm::vec2& contactNormal,
		std::uint32_t& closestPolygonVertexIndex)
	{
		float minSeparationDepth = std::numeric_limits<float>::max();
		float axisDepth = 0.0f;
		float maxPolygonPointOnAxis = 0.0f, minPolygonPointOnAxis = 0.0f, maxCirclePointOnAxis = 0.0f, minCirclePointOnAxis = 0.0f;
		std::uint32_t pointIndex = 0;
		const std::size_t numberOfPointsInPolygon = polygonPoints.size();

		glm::vec2 separatingAxis = glm::vec2(0.0f);

		//Also uses SAT for polygon vs circle collisions
		for (; pointIndex < numberOfPointsInPolygon; pointIndex++)
		{
			//Find normal of edge between current and next point
			const glm::vec2& currentPoint = polygonPoints[pointIndex];
			const glm::vec2& nextPoint = polygonPoints[(pointIndex+1)%numberOfPointsInPolygon];
			const glm::vec2 currentEdge = nextPoint - currentPoint;

			//The normal of the edge is taken as the separating axis
			separatingAxis = glm::vec2(-currentEdge.y, currentEdge.x);
			separatingAxis = glm::normalize(separatingAxis);

			//Using the normal as the new axis, project all edges of both polygons onto that axis and find their
			//max and min projections
			ProjectPointsOnAxis(polygonPoints, separatingAxis, maxPolygonPointOnAxis, minPolygonPointOnAxis);
			ProjectCircleOnAxis(circleCenter, circleRadius, separatingAxis, maxCirclePointOnAxis, minCirclePointOnAxis);

			//Using max and min projections, if a gap is found, return false
			if (minPolygonPointOnAxis > maxCirclePointOnAxis || maxPolygonPointOnAxis < minCirclePointOnAxis)
			{
				return false;
			}

			//Find the magnitude of separation
			axisDepth = std::min(maxCirclePointOnAxis - minPolygonPointOnAxis,
				maxPolygonPointOnAxis - minCirclePointOnAxis);
			if (axisDepth < minSeparationDepth)
			{
				minSeparationDepth = axisDepth;
				contactNormal = separatingAxis;
			}
		}

		float minDistance = std::numeric_limits<float>::max();

		//Now to check for collision from the circle's end, first find the point on the polygon that is closest to the circle.
		for (pointIndex = 0; pointIndex < numberOfPointsInPolygon; ++pointIndex)
		{
			const glm::vec2& point = polygonPoints[pointIndex];
			float pointDistance = glm::distance(point, circleCenter);
			if (pointDistance < minDistance)
			{
				minDistance = pointDistance;
				closestPolygonVertexIndex = pointIndex;
			}
		}

		//Then, the vector from the center to the closest point becomes the separating axis
		separatingAxis = circleCenter - polygonPoints[closestPolygonVertexIndex];
		separatingAxis = glm::normalize(separatingAxis);

		//Project again on new axis
		ProjectPointsOnAxis(polygonPoints, separatingAxis, maxPolygonPointOnAxis, minPolygonPointOnAxis);
		ProjectCircleOnAxis(circleCenter, circleRadius, separatingAxis, maxCirclePointOnAxis, minCirclePointOnAxis);

		//Using max and min projections, if a gap is found, return false
		if (minPolygonPointOnAxis > maxCirclePointOnAxis || maxPolygonPointOnAxis < minCirclePointOnAxis)
		{
			return false;
		}

		//Find the magnitude of separation
		axisDepth = std::min(maxCirclePointOnAxis - minPolygonPointOnAxis, maxPolygonPointOnAxis - minCirclePointOnAxis);
		if (axisDepth < minSeparationDepth)
		{
			minSeparationDepth = axisDepth;
			contactNormal = separatingAxis;
		}

DoDebug(
		glm::vec2 polygonCenterInScreenCoords = Core::ScreenHelperFunctions::WorldToScreen(polygonCenter);
		glm::vec2 polygonEndPointInScreenCoords = Core::ScreenHelperFunctions::WorldToScreen(
			polygonCenter + minSeparationDepth * glm::normalize(contactNormal));
		glm::vec2 circleCenterInScreenCoords = Core::ScreenHelperFunctions::WorldToScreen(circleCenter);
		glm::vec2 circleEndPointInScreenCoords = Core::ScreenHelperFunctions::WorldToScreen(
			circleCenter - minSeparationDepth * glm::normalize(contactNormal));

		SDL_SetRenderDrawColor(Application::GetCoreInstance().GetMainRenderer(), 255, 0, 0, 255);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), polygonCenterInScreenCoords.x,
			polygonCenterInScreenCoords.y, polygonEndPointInScreenCoords.x, polygonEndPointInScreenCoords.y);
		SDL_RenderLine(Application::GetCoreInstance().GetMainRenderer(), circleCenterInScreenCoords.x,
			circleCenterInScreenCoords.y, circleEndPointInScreenCoords.x, circleEndPointInScreenCoords.y);
);

		penetrationDepth = minSeparationDepth;

		glm::vec2 directionVector = circleCenter - polygonCenter;
		if (glm::dot(contactNormal, directionVector) < 0.0f)
		{
			contactNormal = -contactNormal;
		}
		return true;
	}
}