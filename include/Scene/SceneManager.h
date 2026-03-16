#pragma once
#include "Core/GameScene.h"

namespace Scene
{
	class SceneManager
	{
	public:
		SceneManager() = default;
		~SceneManager() = default;

		template<std::derived_from<Core::GameScene> SceneType, uint32_t numberOfEntities>
		Core::GameScene* CreateScene()
		{
			Core::GameScene* gameScene = new SceneType(numberOfEntities);
			return gameScene;
		}

		Core::GameScene* GetCurrentScene();
		void MakeSceneCurrent(Core::GameScene* gameScene);
		void NotifyCurrentSceneForDelete();
		void NotifySceneForDelete(Core::GameScene* scene);

	private:
		Core::GameScene* m_currentScene = nullptr;
	};
}
