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
            if (rotationDirtyByte)
            {
                up = glm::vec2(-std::sin(Rotation), std::cos(Rotation));
                rotationDirtyByte = false;
            }
            return up;
        }

        const glm::vec2& GetRightVector()
        {
            if (rotationDirtyByte)
            {
                right = glm::vec2(std::cos(Rotation), std::sin(Rotation));
                rotationDirtyByte = false;
            }
            return right;
        }

        void SetLocalRotation(float radians)
        {
            if (radians - Rotation > 0.001f)
            {
                rotationDirtyByte = true;
            }

            Rotation = radians;
        }

    public:
        glm::vec2 Position = glm::vec2(0.0f);
        glm::vec2 Scale = glm::vec2(1.0f);
        float Rotation = 0.0f;

    private:
        glm::vec2 up = glm::vec2(0.0f);
        glm::vec2 right = glm::vec2(0.0f);

        bool rotationDirtyByte = false;
    };
}
