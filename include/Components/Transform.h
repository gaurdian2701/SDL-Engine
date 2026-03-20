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

        ~Transform()
        {
            ParentEntity = Core::INVALID_ENTITY_ID;
        }

        void SetParent(Scene::GameObject* parent)
        {
            ParentEntity = parent->GetEntityID();
        }

        glm::vec2 WorldPosition = glm::vec2(0.0f);
        glm::vec2 WorldScale = glm::vec2(1.0f);
        glm::vec2 LocalPosition = glm::vec2(0.0f);
        glm::vec2 LocalScale = glm::vec2(1.0f);
        glm::vec2 Up = glm::vec2(0.0f);
        glm::vec2 Right = glm::vec2(0.0f);

        float LocalRotation = 0.0f;
        float WorldRotation = 0.0f;
        std::uint32_t ParentEntity = Core::INVALID_ENTITY_ID;
    };
}
