#pragma once
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_video.h"

class Renderer;
class Window;

class Application
{
public:
	Application();
	virtual ~Application();
	Application(const Application& other) = delete;
	virtual void BeginApplication() = 0;
	virtual void UpdateApplication(const float deltaTime) = 0;
	virtual void EndApplication() = 0;

	static Application& GetCoreInstance();

	void Init();
	void Run();

	[[nodiscard]] SDL_Window* GetMainWindow() const;
	[[nodiscard]] SDL_Renderer* GetMainRenderer() const;

	void SetBackgroundColor(const SDL_Color someColor)
	{
		m_backgroundColor = someColor;
	}

	SDL_Color GetBackgroundColor() const{ return m_backgroundColor; }

private:
	void InitiateShutdown();
	void RefreshBackground();
	void CheckForQuitEvent();
	void UpdateCoreSystems();

#ifdef _DEBUG
	void StartNewImGUIFrame();
	void PresentImGuiFrame();
#endif

public:
	static const int SCREEN_WIDTH = 1000;
	static const int SCREEN_HEIGHT = 800;

protected:
	SDL_Window* m_mainWindow = nullptr;
	SDL_Renderer* m_mainRenderer = nullptr;
	bool m_isRunning = true;

private:
	SDL_Color m_backgroundColor = { 0, 0, 0, 255 };
	SDL_Event m_mainEventCatcher = SDL_Event {};
};

extern Application* GetApplicationInstance();






