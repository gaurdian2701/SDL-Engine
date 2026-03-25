#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "gtc/quaternion.hpp"
#include "Scene/GameObject.h"

namespace Components
{
    struct Transform
    {
        //Default Constructor
        Transform() = default;
        ~Transform() = default;

        const glm::vec2& GetUpVector()
        {
            if (m_rotationDirtyByte)
            {
                m_up = glm::vec2(-std::sin(Rotation), std::cos(Rotation));
                m_rotationDirtyByte = false;
            }
            return m_up;
        }

        const glm::vec2& GetRightVector()
        {
            if (m_rotationDirtyByte)
            {
                m_right = glm::vec2(std::cos(Rotation), std::sin(Rotation));
                m_rotationDirtyByte = false;
            }
            return m_right;
        }

        void SetLocalRotation(float radians)
        {
            if (radians - Rotation > 0.001f)
            {
                m_rotationDirtyByte = true;
            }

            Rotation = radians;
        }

    public:
        glm::vec2 Position = glm::vec2(0.0f);
        glm::vec2 Scale = glm::vec2(1.0f);
        float Rotation = 0.0f;
        bool Static = false;

    private:
        glm::vec2 m_up = glm::vec2(0.0f);
        glm::vec2 m_right = glm::vec2(0.0f);

        bool m_rotationDirtyByte = false;
    };
}
