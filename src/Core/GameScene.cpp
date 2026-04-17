#include "Core/GameScene.h"
#include <filesystem>
#include "Components/ParticleEmitter.h"
#include "Scene/GameObject.h"
#include "Components/Transform.h"
#include "Components/Renderer2D.h"
#include "imgui.h"
#include "Components/PolygonCollider2D.h"
#include "Components/CircleCollider2D.h"
#include "Components/Rigidbody2D.h"
#include "Components/UITexture.h"
#include "Core/ScreenHelperFunctions.h"
#include "Core/CoreSystems/InputSystem.h"
#include "Core/CoreSystems/ResourceManager.h"
#include "Scene/SceneManager.h"

Core::GameScene::GameScene(const std::uint32_t maxEntitiesInScene)
{
	ECS::ECSManager::GetInstance().InitializeManager(300);
	m_resourceManager = new CoreSystems::ResourceManager();
	RegisterComponents();

	m_gameObjectsInScene.reserve(maxEntitiesInScene);
	m_minCartesianLimits = ScreenHelperFunctions::GetMinCartesianLimits();
	m_maxCartesianLimits = ScreenHelperFunctions::GetMaxCartesianLimits();
	m_maxEntityCount = maxEntitiesInScene;
}

void Core::GameScene::InitializeScene()
{
	CreateGameObjects();
	InitializeGameObjectReferences();
}

void Core::GameScene::RegisterComponents()
{
	GetECSManager().RegisterComponent<Components::Transform>();
	GetECSManager().RegisterComponent<Components::Renderer2D>();
	GetECSManager().RegisterComponent<Components::ParticleEmitter>();
	GetECSManager().RegisterComponent<Components::PolygonCollider2D>();
	GetECSManager().RegisterComponent<Components::CircleCollider2D>();
	GetECSManager().RegisterComponent<Components::Rigidbody2D>();
	GetECSManager().RegisterComponent<Components::UITexture>();
}

void Core::GameScene::InitializeGameObject(Scene::GameObject *someGameObject)
{
	someGameObject->m_sceneReference = this;
	someGameObject->m_entityID = GetECSManager().GenerateEntityID();
	someGameObject->AddComponentsBeforeStartup();
	m_startQueue.push_back(someGameObject);
}

void Core::GameScene::RemoveComponentFromEntityUsingTypeIndex(const std::uint32_t someEntityID,
                                                              const std::size_t someComponentTypeIndex)
{
	ECS::ECSManager::GetInstance().GetComponentRemovalHandlesArray()[someComponentTypeIndex](ECS::ECSManager::GetInstance(), someEntityID);
}

void Core::GameScene::AddComponentToGameObjectData(Scene::GameObject &someGameObject,
                                                   const std::size_t someComponentTypeIndex)
{
	someGameObject.m_componentBitSet[someComponentTypeIndex] = true;
}

void Core::GameScene::RemoveComponentFromGameObjectData(Scene::GameObject &someGameObject,
                                                        const std::size_t someComponentTypeIndex)
{
	someGameObject.m_componentBitSet[someComponentTypeIndex] = false;
}

void Core::GameScene::Start()
{
	//Start GameObjects
	for (auto gameObject: m_gameObjectsInScene) {
		gameObject->Start();
		auto gameObjectInQueue = std::find(m_startQueue.begin(), m_startQueue.end(), gameObject);
		if (*gameObjectInQueue != nullptr && gameObjectInQueue != m_startQueue.end()) {
			m_startQueue.erase(gameObjectInQueue);
		}
	}

	//Start ECS Systems
	ECS::ECSManager::GetInstance().BeginSystems();
}

glm::vec2 Core::GameScene::GetMouseWorldCoords()
{
	const glm::vec2& mouseWorldCoords = ScreenHelperFunctions::ScreenToWorld(Input::InputSystem::GetInstance().GetMouseScreenCoords());
	return ScreenHelperFunctions::ScreenToWorld(mouseWorldCoords);
}

void Core::GameScene::Update(const float deltaTime)
{
	if (m_sceneEndTriggered)
	{
		CleanupScene();
		Application::GetSceneManager().NotifySceneForDelete(this);
		return;
	}

	//Start any newly created GameObjects in the start queue
	for (auto gameObject: m_startQueue)
	{
		gameObject->Start();
	}

	//Clear the start queue
	if (!m_startQueue.empty())
	{
		m_startQueue.clear();
	}

#ifdef _DEBUG
	if (!m_sceneIsPaused)
	{
		for (auto gameObject: m_gameObjectsInScene)
		{
			gameObject->UpdateObject(deltaTime);
		}
	}

	//Update ECS Manager
	ECS::ECSManager::GetInstance().DebugUpdateManager(deltaTime, m_sceneIsPaused);

#else
	//Update GameObjects
	for (auto gameObject: m_gameObjectsInScene)
	{
		gameObject->UpdateObject(deltaTime);
	}

	//Update ECS Manager
	ECS::ECSManager::GetInstance().UpdateManager(deltaTime);
#endif

	GarbageCollect();
}

Core::ECS::ECSManager &Core::GameScene::GetECSManager()
{
	return ECS::ECSManager::GetInstance();
}

Scene::GameObject* Core::GameScene::GetGameObjectFromEntityID(std::uint32_t someEntityID)
{
	auto gameObjectIterator = std::find_if(m_gameObjectsInScene.begin(),
		m_gameObjectsInScene.end(),
		[&](Scene::GameObject* gameObjectInList)
		{
			if (gameObjectInList->GetEntityID() == someEntityID) {
				return true;
			}
			return false;
		});

	if (gameObjectIterator != m_gameObjectsInScene.end() && *gameObjectIterator != nullptr)
	{
		return *gameObjectIterator;
	}
	return nullptr;
}

Scene::GameObject *Core::GameScene::SearchGameObjectByType(const type_info &typeInfo, std::vector<Scene::GameObject *> &someList)
{
	auto gameObjectFound = std::find_if(someList.begin(), someList.end(),
	[&](Scene::GameObject* gameObjectInList)
	{
		if (typeid(*gameObjectInList) == typeInfo)
		{
			return true;
		}
		return false;
	});
	if (gameObjectFound != someList.end())
	{
		return *gameObjectFound;
	}
	return nullptr;
}

void Core::GameScene::DeleteGameObject(Scene::GameObject *someGameObject)
{
	if (someGameObject != nullptr)
	{
		someGameObject->m_isActive = false;

		//Use Component ID to remove components from gameobject, hence untracking them in the ECS
		for (std::size_t typeIndex = 0; typeIndex < MAX_COMPONENT_TYPES; typeIndex++) {
			if (someGameObject->m_componentBitSet[typeIndex]) {
				RemoveComponentFromEntityUsingTypeIndex(someGameObject->m_entityID, typeIndex);
				someGameObject->m_componentBitSet[typeIndex] = false;
			}
		}

		UnTrackGameObject(someGameObject); //Untrack entityID
		someGameObject->m_sceneReference = nullptr;

		auto gameObjectIterator = std::find_if(m_gameObjectsInScene.begin(), m_gameObjectsInScene.end(),
		[&](Scene::GameObject *gameObjectInList)
		{
			if (gameObjectInList == someGameObject)
			{
				return true;
			}
			return false;
		});

		if (gameObjectIterator != m_gameObjectsInScene.end()) {
			*gameObjectIterator = nullptr;
		}

		delete someGameObject;
	}
}

void Core::GameScene::UnTrackGameObject(Scene::GameObject *someGameObject)
{
	ECS::ECSManager::GetInstance().FreeEntityID(someGameObject->GetEntityID());
}

void Core::GameScene::GarbageCollect()
{
	//Erase any gameobjects that are deleted from the scene list
	int gameObjectIndexInSceneList = 0;
	std::vector<Scene::GameObject *>::const_iterator first = m_gameObjectsInScene.begin();

	for (auto &gameObject: m_gameObjectsInScene) {
		if (gameObject == nullptr) {
			m_gameObjectsInScene.erase(first + gameObjectIndexInSceneList);
		}
		gameObjectIndexInSceneList++;
	}
}

void Core::GameScene::CleanupScene()
{
	for (auto &gameObject: m_gameObjectsInScene) {
		gameObject->End();
	}

	for (auto &gameObject: m_gameObjectsInScene) {
		DeleteGameObject(gameObject);
	}

	m_gameObjectsInScene.erase(m_gameObjectsInScene.begin(), m_gameObjectsInScene.end());
	m_resourceManager->ClearData();
	ECS::ECSManager::GetInstance().CleanupManager();
}

void Core::GameScene::SetupForEnd()
{
	m_sceneEndTriggered = true;
}

