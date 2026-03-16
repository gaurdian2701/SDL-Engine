#include "Scene/SceneManager.h"

Core::GameScene *Scene::SceneManager::GetCurrentScene()
{
	return m_currentScene;
}

void Scene::SceneManager::NotifySceneForDelete(Core::GameScene* scene)
{
	if (scene != nullptr)
	{
		if (m_currentScene == scene)
		{
			m_currentScene = nullptr;
		}
		delete scene;
	}
}

void Scene::SceneManager::NotifyCurrentSceneForDelete()
{
	NotifySceneForDelete(m_currentScene);
}

void Scene::SceneManager::MakeSceneCurrent(Core::GameScene* gameScene)
{
	if (m_currentScene == nullptr && gameScene != nullptr)
	{
		m_currentScene = gameScene;
	}
}