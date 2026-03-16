#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include "gtc/quaternion.hpp"
#include "PrintDebug.h"
#include "Scene/GameObject.h"

namespace Assets::Components
{
    struct Transform
    {
        //Default Constructor
        Transform() = default;

        ~Transform()
        {
            ParentEntity = Core::INVALID_ENTITY_ID;
        }

        //Copy Constructor
        Transform(const Transform& other)
        {
            WorldPosition = other.WorldPosition;
            WorldScale = other.WorldScale;
            WorldRotation = other.WorldRotation;

            LocalPosition = other.LocalPosition;
            LocalScale = other.LocalScale;
            LocalRotation = other.LocalRotation;

            ParentEntity = other.ParentEntity;
            Owner = other.Owner;
        }

        //Move Constructor
        Transform(Transform&& other) noexcept
        {
            if (this != &other)
            {
                WorldPosition = other.WorldPosition;
                WorldScale = other.WorldScale;
                WorldRotation = other.WorldRotation;

                LocalPosition = other.LocalPosition;
                LocalScale = other.LocalScale;
                LocalRotation = other.LocalRotation;

                ParentEntity = other.ParentEntity;
                Owner = other.Owner;
            }
        }

        //Copy Assignment operator
        Transform& operator=(const Transform& other)
        {
            if (this != &other)
            {
                WorldPosition = other.WorldPosition;
                WorldScale = other.WorldScale;
                WorldRotation = other.WorldRotation;

                LocalPosition = other.LocalPosition;
                LocalScale = other.LocalScale;
                LocalRotation = other.LocalRotation;

                ParentEntity = other.ParentEntity;
                Owner = other.Owner;
            }

            return *this;
        }

        //Move Assignment operator
        Transform& operator=(Transform&& other) noexcept
        {
            if (this != &other)
            {
                WorldPosition = other.WorldPosition;
                WorldScale = other.WorldScale;
                WorldRotation = other.WorldRotation;

                LocalPosition = other.LocalPosition;
                LocalScale = other.LocalScale;
                LocalRotation = other.LocalRotation;

                ParentEntity = other.ParentEntity;
                Owner = other.Owner;
            }
            return *this;
        }

        void SetParent(Scene::GameObject* parent)
        {
            ParentEntity = parent->GetEntityID();
        }

        Scene::GameObject* Owner = nullptr;

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
