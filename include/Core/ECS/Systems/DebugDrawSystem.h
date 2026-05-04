#pragma once
#include "Core/ECS/Systems/System.h"

namespace Core::ECS::Systems
{
    struct LineDrawData
    {
        glm::vec2 StartPos = glm::vec2(0.0f);
        glm::vec2 EndPos = glm::vec2(0.0f);
    };

    struct CircleDrawData
    {
        glm::vec2 CenterWorldPos = glm::vec2(0.0f);
        float radius = 0.0f;
    };

    struct PolygonDrawData
    {
        const std::vector<glm::vec2>* Points = nullptr;
    };

    union ShapeData
    {
        LineDrawData line;
        CircleDrawData circle;
        PolygonDrawData polygon;
    };

    enum class ShapeType : uint8_t
    {
        LINE,
        CIRCLE,
        POLYGON
    };

    struct DebugDrawData
    {
        DebugDrawData(const LineDrawData& someLine, float red, float green, float blue, float alpha) : shapeData{.line = someLine},
        shapeType(ShapeType::LINE),
        r(red),
        g(green),
        b(blue),
        a(alpha)
        {
        }

        DebugDrawData(const CircleDrawData& someCircle, float red, float green, float blue, float alpha) : shapeData{.circle = someCircle},
        shapeType(ShapeType::CIRCLE),
        r(red),
        g(green),
        b(blue),
        a(alpha)
        {
        }

        DebugDrawData(const PolygonDrawData& somePolygon, float red, float green, float blue, float alpha) : shapeData{.polygon = somePolygon},
        shapeType(ShapeType::POLYGON),
        r(red),
        g(green),
        b(blue),
        a(alpha)
        {
        }

        ShapeData shapeData;
        ShapeType shapeType;

        float r = 0, g = 0, b = 0, a = 255;
    };

    class DebugDrawSystem : public System
    {
    public:
        DebugDrawSystem() = default;
        ~DebugDrawSystem() override = default;

        void RegisterInterestedComponents() override;
        void UpdateSystem(const float deltaTime) override;

        void DrawDebugLine(const glm::vec2& startPos,
            const glm::vec2& endPos,
            float r,
            float g,
            float b,
            float a);

        void DrawDebugHollowCircle(const glm::vec2& centerWorldPos,
        const float radius,
        float r,
        float g,
        float b,
        float a);

        void DrawDebugPolygon(std::vector<glm::vec2>& points,
        float r,
        float g,
        float b,
        float a);

        void DrawFromDebugDrawList();

    private:
        std::vector<DebugDrawData> m_drawList = std::vector<DebugDrawData>();
    };
}
