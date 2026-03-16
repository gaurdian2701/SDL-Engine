#pragma once
#include <cstdint>
#include "Core/GameScene.h"

namespace Scene
{
    class GameObject
    {
    public:
        //Making a default constructor grants some flexibility to create objects not tracked by the scene as well?
        GameObject() = default;
        virtual ~GameObject() = default;

        virtual void AddComponentsBeforeStartup(){}

        virtual void Start(){}

        void UpdateObject(const float deltaTime)
        {
            if (m_isActive)
            {
                Update(deltaTime);
            }
        }

        virtual void Update(const float deltaTime){}
        [[nodiscard]] std::uint32_t GetEntityID() const { return m_entityID;}

        virtual void End(){}

        template<typename T>
        void AddComponent()
        {
            m_sceneReference->AddComponentToEntity<T>(*this, m_entityID);
        }

        template<typename T>
        void RemoveComponent()
        {
            m_sceneReference->RemoveComponentFromEntity<T>(*this, m_entityID);
        }

        template<typename T>
        T* GetComponent()
        {
            return m_sceneReference->GetECSManager().GetComponent<T>(m_entityID);
        }

        Core::GameScene& GetSceneReference() const
        {
            return *m_sceneReference;
        }

    public:
#ifdef _DEBUG
        std::string m_name = "GameObject";
#endif
        //TODO: Not used for now, but will use this in a GameScene step where I have to
        //set parents according to this pointer
        GameObject* m_parent = nullptr;
        bool m_isActive = true;

    private:
        std::uint32_t m_entityID = Core::INVALID_ENTITY_ID;
        Core::GameScene* m_sceneReference = nullptr;
        std::bitset<Core::MAX_COMPONENT_TYPES> m_componentBitSet;

        friend class Core::GameScene;
    };
}
