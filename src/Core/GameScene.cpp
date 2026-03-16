#include "Core/GameScene.h"
#include <filesystem>
#include "Assets/Components/ParticleEmitter.h"
#include "Scene/GameObject.h"
#include "Assets/Components/Transform.h"
#include "Assets/Components/Renderer2D.h"
#include "imgui.h"
#include "../../../include/EmptyApp.h"
#include "Assets/Components/Collider2D.h"
#include "Assets/Components/UITexture.h"
#include "Core/HelperFunctions.h"
#include "Core/CoreSystems/ResourceManager.h"
#include "Scene/SceneManager.h"

Core::GameScene::GameScene(const std::uint32_t maxEntitiesInScene)
{
	ECS::ECSManager::GetInstance().InitializeManager(300);
	m_resourceManager = new CoreSystems::ResourceManager();
	RegisterComponents();

	m_gameObjectsInScene.reserve(maxEntitiesInScene);
	m_minCartesianLimits = GetMinCartesianLimits();
	m_maxCartesianLimits = GetMaxCartesianLimits();
	m_maxEntityCount = maxEntitiesInScene;
}

void Core::GameScene::InitializeScene()
{
	CreateGameObjects();
	InitializeGameObjectReferences();
}

void Core::GameScene::RegisterComponents()
{
	ECS::ECSManager::GetInstance().RegisterComponent<Assets::Components::Transform>();
	ECS::ECSManager::GetInstance().RegisterComponent<Assets::Components::Renderer2D>();
	ECS::ECSManager::GetInstance().RegisterComponent<Assets::Components::ParticleEmitter>();
	ECS::ECSManager::GetInstance().RegisterComponent<Assets::Components::Collider2D>();
	ECS::ECSManager::GetInstance().RegisterComponent<Assets::Components::UITexture>();
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

	//Update GameObjects
	for (auto gameObject: m_gameObjectsInScene)
	{
		gameObject->UpdateObject(deltaTime);
	}

	//Update ECS Manager
	ECS::ECSManager::GetInstance().UpdateManager(deltaTime);

#ifdef _DEBUG
	UpdateImGuiDebugs();
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
			                                       if (gameObjectInList == someGameObject) {
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

#ifdef _DEBUG
void Core::GameScene::UpdateImGuiDebugs()
{
	ImGui::Begin("Debug");
	ImGui::Text("FRAME RATE:");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)\n",
	            1000.0f / ImGui::GetIO().Framerate,
	            ImGui::GetIO().Framerate);
	ImGui::Separator();
	ImGui::Text("Number of Entities: %i\n", m_gameObjectsInScene.size());
	ImGui::Separator();
	ImGui::Text("CHANGEABLE DEBUGS: ");

	for (auto &gameObject: m_gameObjectsInScene) {
		Assets::Components::Transform *transform = gameObject->GetComponent<Assets::Components::Transform>();

		if (transform != nullptr) {
			ImGui::PushID(gameObject->m_entityID);
			ImGui::Text(gameObject->m_name.c_str());
			ImGui::DragFloat2("Position", &transform->LocalPosition.x, 0.5f, -100.0f, 100.0f);
			ImGui::DragFloat2("Scale", &transform->LocalScale.x, 0.5f, -100.0f, 100.0f);
			ImGui::DragFloat("Rotation", &transform->LocalRotation, 0.5f, -100.0f, 100.0f);
			ImGui::Separator();
			ImGui::PopID();
		}
	}
	ImGui::End();
}
#endif

void Core::GameScene::SetupForEnd()
{
	m_sceneEndTriggered = true;
}

