#pragma once
#include "vec2.hpp"
#include "Components/PolygonCollider2D.h"

namespace Core::Physics
{
    static std::uint8_t ClipLineSegmentToPoint(const glm::vec2 p1,
        const glm::vec2 p2,
        const glm::vec2& normal,
        const glm::vec2& offset,
        std::vector<glm::vec2>& clippedPoints)
    {
        std::uint8_t contactPointIndex = 0;

        float distance1 = glm::dot(p1-offset, normal);
        float distance2 = glm::dot(p2-offset, normal);

        if (distance1 <= 0.0f)
        {
            clippedPoints[contactPointIndex] = p1;
            ++contactPointIndex;
        }
        if (distance2 <= 0.0f)
        {
            clippedPoints[contactPointIndex] = p2;
            ++contactPointIndex;
        }

        //Edge case where there is only one contact point and it has passed through the reference edge
        if (MathHelpers::GetSign(distance1) != MathHelpers::GetSign(distance2) && contactPointIndex < 2)
        {
            float percentageAcrossLineSegment = distance2 / (distance2 - distance1);
            glm::vec2 intersectionPoint = p2 + (p1 - p2) * percentageAcrossLineSegment;
            clippedPoints[contactPointIndex] = intersectionPoint;
            ++contactPointIndex;
        }

        return contactPointIndex;
    }

    //Uses Sutherland-Hodgeman clipping
    static void FindPolygonVsPolygonContactPoints(const Components::PolygonCollider2D& polygonA,
        const Components::PolygonCollider2D& polygonB,
        const bool aHasReferenceEdge,
        const glm::vec2& referenceEdge,
        const glm::vec2& contactNormal,
        const std::size_t& referenceEdgeIndex,
        std::vector<glm::vec2>& contactPoints,
        uint8_t& numberOfContactPoints)
    {
        float lowestDotProduct = std::numeric_limits<float>::max();

        const auto& referencePolygonPoints = aHasReferenceEdge ? polygonA.GetVertices() : polygonB.GetVertices();
        const auto& incidentPolygonPoints = aHasReferenceEdge ? polygonB.GetVertices() : polygonA.GetVertices();

        std::size_t numberOfPointsInIncidentPolygon = incidentPolygonPoints.size();
        std::size_t numberOfPointsInReferencePolygon = referencePolygonPoints.size();
        glm::vec2 referenceEdgeTangent = glm::normalize(referenceEdge);
        glm::vec2 referenceEdgeNormal = glm::vec2(-referenceEdgeTangent.y, referenceEdgeTangent.x);
        glm::vec2 referenceEdgePointA = referencePolygonPoints[referenceEdgeIndex];
        glm::vec2 referenceEdgePointB = referencePolygonPoints[(referenceEdgeIndex + 1) % numberOfPointsInReferencePolygon];
        float referenceEdgeLength = glm::length(referenceEdge);
        std::size_t incidentIndex = 0;

        //First find the incident edge. This is the edge whose normal is the most anti-parallel/opposite
        //to the reference edge normal, i.e. the edge that is most "facing" the reference edge
        for (std::size_t pointIndex = 0; pointIndex < numberOfPointsInIncidentPolygon; ++pointIndex)
        {
            glm::vec2 currentEdge = incidentPolygonPoints[(pointIndex + 1) % numberOfPointsInIncidentPolygon] -
                incidentPolygonPoints[pointIndex];

            glm::vec2 currentNormal = glm::vec2(-currentEdge.y, currentEdge.x);
            currentNormal = glm::normalize(currentNormal);
            float currentDotProduct = glm::dot(currentNormal, referenceEdgeNormal);

            if ( currentDotProduct < lowestDotProduct)
            {
                lowestDotProduct = currentDotProduct;
                incidentIndex = pointIndex;
            }
        }

        glm::vec2 incidentEdgePointA = incidentPolygonPoints[incidentIndex];
        glm::vec2 incidentEdgePointB = incidentPolygonPoints[(incidentIndex + 1) % numberOfPointsInIncidentPolygon];

        //Next, we start clipping the incident edge.
        //First check if the incident edge is within the length of the reference edge. If not, we can cancel out early.
        //We do this by projecting the points on the incident edge onto the reference edge.
        float projectionA = glm::dot(incidentEdgePointA - referenceEdgePointA, referenceEdgeTangent);
        float projectionB = glm::dot(incidentEdgePointB - referenceEdgePointA, referenceEdgeTangent);
        float minProjection = std::min(projectionA, projectionB);
        float maxProjection = std::max(projectionA, projectionB);

        //If our incident edge is out of the bounds of the reference edge, cancel out early.
        if (maxProjection < 0.0f|| minProjection > referenceEdgeLength)
        {
            return;
        }

        //Clip the points within the reference edge, a1->a2
        if (contactPoints.size() != 2)
        {
            contactPoints = std::vector<glm::vec2>(2);
        }

        //Start with reference edge point A, cancel early if we do not find any contact points
        numberOfContactPoints = ClipLineSegmentToPoint(incidentEdgePointA,
            incidentEdgePointB,
            -referenceEdgeTangent,
            referenceEdgePointA,
            contactPoints);

        if (numberOfContactPoints == 0)
        {
            return;
        }

        //Then with reference edge point B
        numberOfContactPoints = ClipLineSegmentToPoint(contactPoints[0],
        contactPoints[1],
        referenceEdgeTangent,
        referenceEdgePointB,
        contactPoints);

        //Finally, keep points that are behind/passed through the reference face, use slop
        uint8_t newCount = 0;

        for (uint8_t i = 0; i < numberOfContactPoints; i++)
        {
            float separation = glm::dot(contactPoints[i] - referenceEdgePointA, referenceEdgeNormal);

            if (separation <= 0.0f)
            {
                contactPoints[newCount++] = contactPoints[i];
            }
        }
        numberOfContactPoints = newCount;
    }

    static void FindCircleVsCircleContactPoint(const Components::CircleCollider2D& circleA,
        const Components::CircleCollider2D& circleB,
        glm::vec2& contactPoint)
    {
        glm::vec2 directionVector = glm::normalize(circleB.GetCenter() - circleA.GetCenter());
        contactPoint = circleA.GetCenter() + directionVector * circleA.GetRadius();
    }

    static void FindPolygonVsCircleContactPoint(const Components::PolygonCollider2D& polygon,
        const Components::CircleCollider2D& circle,
        glm::vec2& contactPoint,
        std::uint32_t closestVertexIndex)
    {
        auto& polygonPoints = polygon.GetVertices();
        std::size_t numberOfPointsInPolygon = polygon.GetVertices().size();
        const glm::vec2& circleCenter = circle.GetCenter();

        //Compute the closest edge to circle -
        //We would compare the two closest edges from the closest vertex B, i.e. edges AB and BC
        const glm::vec2& closestEdgeVertexA = polygonPoints[(closestVertexIndex - 1) % numberOfPointsInPolygon];
        const glm::vec2& closestEdgeVertexB = polygonPoints[closestVertexIndex];
        const glm::vec2& closestEdgeVertexC = polygonPoints[(closestVertexIndex+1) % numberOfPointsInPolygon];

        glm::vec2 closestPointOnAB = glm::vec2(0.0f), closestPointOnBC = glm::vec2(0.0f);

        float distanceFromEdgeAB = MathHelpers::PointLineSegmentDistance(closestEdgeVertexA,
            closestEdgeVertexB,
            circleCenter,
            closestPointOnAB);

        float distanceFromEdgeBC = MathHelpers::PointLineSegmentDistance(closestEdgeVertexB,
            closestEdgeVertexC,
            circleCenter,
            closestPointOnBC);

        if (distanceFromEdgeAB < distanceFromEdgeBC)
        {
            contactPoint = closestPointOnAB;
        }
        else
        {
            contactPoint = closestPointOnBC;
        }
    }
}
