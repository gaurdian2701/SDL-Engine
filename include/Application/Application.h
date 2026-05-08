#pragma once
#include "DebugStatements.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

namespace Core
{
	class GameScene;
}

namespace Core
{
	class Editor;
}

namespace Scene
{
	class SceneManager;
}

class Renderer;
class Window;

class Application
{
public:
	Application() = default;
	virtual ~Application()
	{
		m_mainWindow = nullptr;
	}

	Application(const Application& other) = delete;
	virtual void BeginApplication() = 0;
	virtual void UpdateApplication(const float deltaTime) = 0;
	virtual void EndApplication() = 0;
	void StopRunning() { m_isRunning = false; }

	static Application& GetCoreInstance();
	static Scene::SceneManager& GetSceneManager();
	float GetDeltaTime() const;

	virtual void Init();
	void Run();

	[[nodiscard]] SDL_Window* GetMainWindow() const;
	[[nodiscard]] SDL_Renderer* GetMainRenderer() const;
	[[nodiscard]] int GetScreenWidth() const
	{
		return m_screenWidth;
	}
	[[nodiscard]] int GetScreenHeight() const
	{
		return m_screenHeight;
	}

	void SetBackgroundColor(const SDL_Color someColor)
	{
		m_backgroundColor = someColor;
	}

	SDL_Color GetBackgroundColor() const{ return m_backgroundColor; }

protected:
	virtual void StartWindow();
	virtual void StartTTF();
	DoDebug(virtual void StartImGuiDebug());

private:
	void InitiateShutdown();
	void RefreshBackground();
	void UpdateCoreSystems();

	DoDebug(void StartNewImGuiFrame());
	DoDebug(void PresentImGuiFrame());

public:
	int m_screenWidth = 0;
	int m_screenHeight = 0;

protected:
	SDL_Window* m_mainWindow = nullptr;
	SDL_Renderer* m_mainRenderer = nullptr;
	const char *m_projectName = "SDL-Project";

	DoDebug(Core::Editor* m_editor = nullptr);

	bool m_isRunning = true;

private:
	SDL_Color m_backgroundColor = SDL_Color();
	SDL_Event m_mainEventCatcher = SDL_Event {};
	float m_deltaTime = 0.0f;
};

extern Application* GetApplicationInstance();






