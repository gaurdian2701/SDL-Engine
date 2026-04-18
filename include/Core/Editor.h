#pragma once
#include <cstdint>
#include "imgui.h"
#include "Core/ECS/ECSData.h"

namespace Core
{
    class Editor
    {
    public:
        Editor() = default;
        ~Editor() = default;

        float GetDeltaTimeInput()
        {
            return m_deltaTimeInput;
        }

        virtual void Update(const float deltaTime);

    protected:
        std::uint32_t m_draggedEntity = INVALID_ENTITY_ID;
        bool m_isDragging = false;
        ImVec2 m_dragOffset = ImVec2(0.f, 0.f);
        float m_deltaTimeInput = 240.0f;
    };
}
