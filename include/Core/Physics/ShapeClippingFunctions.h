#pragma once
#include "vec2.hpp"
#include "Components/PolygonCollider2D.h"

namespace Core::Physics
{
    static std::vector<glm::vec2> ClipLineSegmentToLine(const glm::vec2& p1,
        const glm::vec2& p2,
        const glm::vec2& normal,
        const glm::vec2& offset)
    {
        float distance1 = glm::dot(p1-offset, normal);
        float distance2 = glm::dot(p2-offset, normal);

        std::vector<glm::vec2> clippedPoints = std::vector<glm::vec2>(2);

        if (distance1 <= 0.0f)
        {
            clippedPoints.push_back(p1);
        }
        if (distance2 <= 0.0f)
        {
            clippedPoints.push_back(p2);
        }

        if (std::copysign(1.0f, distance1) != std::copysign(1.0f, distance2) && clippedPoints.size() < 2)
        {
            float percentageAcrossLineSegment = distance2 / (distance2 - distance1);
            glm::vec2 intersectionPoint = p2 + (p1 - p2) * percentageAcrossLineSegment;
            clippedPoints.push_back(intersectionPoint);
        }

        return clippedPoints;
    }

    static void ClipPolygonVsPolygon(Components::PolygonCollider2D& polygonA,
        Components::PolygonCollider2D& polygonB,
        bool aHasReferenceEdge,
        const glm::vec2& referenceEdge,
        std::size_t& referenceEdgeIndex)
    {
        float lowestDotProduct = std::numeric_limits<float>::max();

        const auto& referencePolygonPoints = aHasReferenceEdge ? polygonA.GetPoints() : polygonB.GetPoints();
        const auto& incidentPolygonPoints = aHasReferenceEdge ? polygonB.GetPoints() : polygonA.GetPoints();

        std::size_t numberOfPointsInIncidentPolygon = incidentPolygonPoints.size();
        std::size_t numberOfPointsInReferencePolygon = referencePolygonPoints.size();
        glm::vec2 referenceEdgeNormal = glm::normalize(glm::vec2(-referenceEdge.y, referenceEdge.x));
        glm::vec2 referenceEdgeTangent = glm::normalize(referenceEdge);
        glm::vec2 referenceEdgePointA = referencePolygonPoints[referenceEdgeIndex];
        glm::vec2 referenceEdgePointB = referencePolygonPoints[(referenceEdgeIndex + 1) % numberOfPointsInReferencePolygon];
        float referenceEdgeLength = glm::length(referenceEdge);
        glm::vec2 incidentEdge = glm::vec2(0.0f);
        std::size_t incidentIndex = 0;

        //First find the incident edge. This is the edge whose normal is the most anti-parallel/opposite
        //to the reference edge normal, i.e. the edge that is most "facing" the reference edge
        for (std::size_t pointIndex = 0; pointIndex < numberOfPointsInIncidentPolygon; ++pointIndex)
        {
            glm::vec2 currentEdge = incidentPolygonPoints[pointIndex] - incidentPolygonPoints[
                (pointIndex + numberOfPointsInIncidentPolygon - 1) & numberOfPointsInIncidentPolygon];

            glm::vec2 currentNormal = glm::vec2(-currentEdge.y, currentEdge.x);
            float currentDotProduct = glm::dot(currentNormal, referenceEdgeNormal);

            if ( currentDotProduct < lowestDotProduct)
            {
                lowestDotProduct = currentDotProduct;
                incidentIndex = pointIndex;
            }
        }

        glm::vec2 incidentEdgePointA = incidentPolygonPoints[
                (incidentIndex + numberOfPointsInIncidentPolygon - 1) & numberOfPointsInIncidentPolygon];
        glm::vec2 incidentEdgePointB = incidentPolygonPoints[incidentIndex];

        //Next, we start clipping the incident edge.
        //First check if the incident edge is within the length of the reference edge. If not, we can cancel out early.
        //We do this by projecting the points on the incident edge onto the reference edge.
        float projectionA = glm::dot(incidentEdgePointA - referenceEdgePointA, referenceEdgeTangent);
        float projectionB = glm::dot(incidentEdgePointB - referenceEdgePointA, referenceEdgeTangent);
        float minProjection = std::min(projectionA, projectionB);
        float maxProjection = std::max(projectionA, projectionB);

        //If our incident edge is out of the bounds of the reference edge, cancel out early.
        if (maxProjection < 0 || minProjection > referenceEdgeLength)
        {
            return;
        }

        std::vector<glm::vec2> clippedPoints = ClipLineSegmentToLine(incidentEdgePointA,
            incidentEdgePointB,
            referenceEdgeNormal,
            referenceEdgePointA);

        if (clippedPoints.empty())
        {
            return;
        }


    }
}
