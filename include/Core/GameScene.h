#pragma once
#include "vec2.hpp"
#include "Application/Application.h"
#include "Core/ECS/ECSManager.h"
#include "PrintDebug.h"
#include "CoreSystems/ResourceManager.h"

namespace Scene
{
    class GameObject;
}

namespace Core
{
    class GameScene
    {
    public:
        explicit GameScene(const std::uint32_t maxEntitiesInScene);
        virtual ~GameScene()
        {
            delete m_resourceManager;
        }

        virtual void CreateGameObjects() = 0;
        virtual void InitializeScene();

        virtual void Start();
        virtual void Update(const float deltaTime);

        ECS::ECSManager& GetECSManager();
        CoreSystems::ResourceManager& GetResourceManager()
        {
            return *m_resourceManager;
        }

        void DeleteGameObject(Scene::GameObject* someGameObject);
        inline void UnTrackGameObject(Scene::GameObject* someGameObject);
        void GarbageCollect();
        void SetupForEnd();
        bool HasEnded() const
        {
            return m_sceneEndTriggered;
        }

#ifdef _DEBUG
#endif

        template<typename T>
        void AddComponentToEntity(Scene::GameObject& someGameObject, const std::uint32_t someEntityID)
        {
            Core::ECS::ECSManager::GetInstance().AddComponent<T>(someEntityID);
            AddComponentToGameObjectData(someGameObject,
                Core::ECS::ECSManager::GetInstance().GetGeneratedComponentTypeIndex<T>(Core::ECS::ECSManager::GetInstance()));
        }

        template<typename T>
        void RemoveComponentFromEntity(Scene::GameObject& someGameObject, const std::uint32_t someEntityID)
        {
            Core::ECS::ECSManager::GetInstance().RemoveComponent<T>(someEntityID);
            RemoveComponentFromGameObjectData(someGameObject,
                Core::ECS::ECSManager::GetInstance().GetGeneratedComponentTypeIndex<T>(Core::ECS::ECSManager::GetInstance()));
        }

        void RemoveComponentFromEntityUsingTypeIndex(const std::uint32_t someEntityID,
            const std::size_t someComponentTypeIndex);

        void AddComponentToGameObjectData(Scene::GameObject& someGameObject, std::size_t someComponentTypeIndex);
        void RemoveComponentFromGameObjectData(Scene::GameObject& someGameObject, std::size_t someComponentTypeIndex);

        template<std::derived_from<Scene::GameObject> GameObjectType, typename ...Args>
        GameObjectType* AddGameObject(Args&&... gameObjectArguments)
        {
            if (m_gameObjectsInScene.size() == m_maxEntityCount)
            {
                PrintDebug("%s", "SCENE MAX ENTITY CAPACITY REACHED");
                return nullptr;
            }

            m_gameObjectsInScene.push_back(new GameObjectType(std::forward<Args>(gameObjectArguments)...));
            m_entityToGameObjectMap[std::type_index(typeid(GameObjectType))].push_back(m_gameObjectsInScene.back());
            InitializeGameObject(m_gameObjectsInScene.back());
            return static_cast<GameObjectType*>(m_gameObjectsInScene.back());
        }

        template<std::derived_from<Scene::GameObject> GameObjectType>
        GameObjectType* GetGameObjectUsingType()
        {
            auto& gameObjectList = m_entityToGameObjectMap[std::type_index(typeid(GameObjectType))];
            return static_cast<GameObjectType*>(SearchGameObjectByType(typeid(GameObjectType), gameObjectList));
        }

        Scene::GameObject* GetGameObjectFromEntityID(std::uint32_t someEntityID);

    protected:
        virtual void InitializeGameObjectReferences(){}
#ifdef _DEBUG
        virtual void UpdateImGuiDebugs();
#endif

    private:
        Scene::GameObject* SearchGameObjectByType(const type_info &typeInfo, std::vector<Scene::GameObject*>& someList);
        void InitializeGameObject(Scene::GameObject* someGameObject);
        void RegisterComponents();
        void CleanupScene();

    protected:
        float m_sceneBoundsRadius;
        std::vector<Scene::GameObject*> m_gameObjectsInScene;

    private:
        bool m_sceneEndTriggered = false;
        std::vector<Scene::GameObject*> m_startQueue = std::vector<Scene::GameObject*>();
        std::unordered_map<std::type_index, std::vector<Scene::GameObject*>> m_entityToGameObjectMap;
        CoreSystems::ResourceManager* m_resourceManager = nullptr;
        uint32_t m_maxEntityCount = 0;
        glm::vec2 m_minCartesianLimits = glm::vec2(0.0f);
        glm::vec2 m_maxCartesianLimits = glm::vec2(0.0f);
    };
}
