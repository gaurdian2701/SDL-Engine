#include "Application/Application.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <SDL3/SDL_init.h>
#include "DebugStatements.h"
#include "Core/Editor.h"
#include "Core/CoreSystems/CoreSystemsHolder.h"
#include "Scene/SceneManager.h"
#include "SDL3_ttf/SDL_ttf.h"

#ifdef _DEBUG
    #include "imgui.h"
    #include "imgui_impl_sdl3.h"
    #include "imgui_impl_sdlrenderer3.h"
#endif

static Application* CoreApplicationInstance = nullptr;
const char* WINDOW_NAME = "SDL-Project";

Application::Application()
{
    if (CoreApplicationInstance != nullptr)
    {
        PrintDebug("Application already exists!");
        return;
    }

    CoreApplicationInstance = this;

    if (SDL_Init(SDL_INIT_VIDEO) == false)
    {
        PrintDebug("SDL_Init Error: %s", SDL_GetError());
        return;
    }

    if (!TTF_Init())
    {
        PrintDebug("TTF Initialization Error\n");
    }

    m_mainWindow = SDL_CreateWindow(
        WINDOW_NAME,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_RESIZABLE);

    SDL_SetWindowMinimumSize(m_mainWindow, SCREEN_WIDTH, SCREEN_HEIGHT);
    m_mainRenderer = SDL_CreateRenderer(m_mainWindow, nullptr);
    assert(m_mainWindow != nullptr && "Window Creation Failed!");

    DoDebugCode(
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDL3_InitForSDLRenderer(m_mainWindow, m_mainRenderer);
    ImGui_ImplSDLRenderer3_Init(m_mainRenderer);
    );
}

Application::~Application()
{
    m_mainWindow = nullptr;
}

Application& Application::GetCoreInstance()
{
    return *CoreApplicationInstance;
}

Scene::SceneManager& Application::GetSceneManager()
{
    static Scene::SceneManager sceneManager = Scene::SceneManager();
    return sceneManager;
}

void Application::Init()
{
    for (auto& system : Core::GetCoreSystems())
    {
        system->Initialize();
    }

    DoDebugStatement(m_editor = new Core::Editor());
}

void Application::InitiateShutdown()
{
DoDebugCode(
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
);

    SDL_DestroyRenderer(m_mainRenderer);
    SDL_DestroyWindow(m_mainWindow);
    TTF_Quit();
    SDL_Quit();
}

float Application::GetDeltaTime() const
{
    return m_deltaTime;
}

void Application::Run()
{
    GetApplicationInstance()->BeginApplication();
    auto lastFrameTime = std::chrono::high_resolution_clock::now();

    SDL_zero(m_mainEventCatcher);
    SDL_SetRenderDrawBlendMode(m_mainRenderer, SDL_BLENDMODE_BLEND);

    while (m_isRunning)
    {
        CheckForQuitEvent();

        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        m_deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();

        DoDebugCode(
        StartNewImGUIFrame();
        );
        RefreshBackground();

        UpdateCoreSystems();
        GetApplicationInstance()->UpdateApplication(m_deltaTime);

        DoDebugCode(
        m_editor->Update(m_deltaTime);
        PresentImGuiFrame();
        );

        SDL_RenderPresent(m_mainRenderer);

        DoDebugCode(
        //Stall until next frame update
        while (std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - lastFrameTime).count() < 1 / m_editor->GetDeltaTimeInput())
        {}
        );

        lastFrameTime = currentFrameTime;
    }

    GetApplicationInstance()->EndApplication();
    InitiateShutdown();
}

void Application::UpdateCoreSystems()
{
    for (auto& system : Core::GetCoreSystems())
    {
        system->Update();
    }
}

void Application::CheckForQuitEvent()
{
    while (SDL_PollEvent(&m_mainEventCatcher))
    {
        if (m_mainEventCatcher.type == SDL_EVENT_QUIT)
        {
            m_isRunning = false;
        }

DoDebugCode(
        ImGui_ImplSDL3_ProcessEvent(&m_mainEventCatcher);
        );
    }
}

void Application::RefreshBackground()
{
    SDL_SetRenderDrawColor(m_mainRenderer, m_backgroundColor.r, m_backgroundColor.g, m_backgroundColor.b, m_backgroundColor.a);
    SDL_RenderClear(m_mainRenderer);
}

#ifdef _DEBUG
void Application::StartNewImGUIFrame()
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void Application::PresentImGuiFrame()
{
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), m_mainRenderer);
}
#endif

SDL_Window* Application::GetMainWindow() const
{
    return m_mainWindow;
}

SDL_Renderer* Application::GetMainRenderer() const
{
    return m_mainRenderer;
}




