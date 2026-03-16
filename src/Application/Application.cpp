#include "Application/Application.h"
#include <cassert>
#include <chrono>
#include <iostream>
#include <SDL3/SDL_init.h>
#include "PrintDebug.h"
#include "Core/CoreSystems/CoreSystemsHolder.h"
#include "SDL3_ttf/SDL_ttf.h"

#ifdef _DEBUG
    #include "imgui.h"
    #include "imgui_impl_sdl3.h"
    #include "imgui_impl_sdlrenderer3.h"
#endif

static Application* CoreApplicationInstance = nullptr;
const char* WINDOW_NAME = "Asteroids";

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

#ifdef _DEBUG
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplSDL3_InitForSDLRenderer(m_mainWindow, m_mainRenderer);
    ImGui_ImplSDLRenderer3_Init(m_mainRenderer);
#endif
}

Application::~Application()
{
    m_mainWindow = nullptr;
}

Application& Application::GetCoreInstance()
{
    return *CoreApplicationInstance;
}

void Application::Init()
{
    for (auto& system : Core::GetCoreSystems())
    {
        system->Initialize();
    }
}

void Application::InitiateShutdown()
{
#ifdef _DEBUG
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
#endif

    SDL_DestroyRenderer(m_mainRenderer);
    SDL_DestroyWindow(m_mainWindow);
    TTF_Quit();
    SDL_Quit();
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
        float deltaTime = std::chrono::duration<float>(currentFrameTime - lastFrameTime).count();
        lastFrameTime = currentFrameTime;

#ifdef _DEBUG
        StartNewImGUIFrame();
#endif
        RefreshBackground();

        UpdateCoreSystems();
        GetApplicationInstance()->UpdateApplication(deltaTime);

#ifdef _DEBUG
        PresentImGuiFrame();
#endif

        SDL_RenderPresent(m_mainRenderer);
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

#ifdef _DEBUG
        ImGui_ImplSDL3_ProcessEvent(&m_mainEventCatcher);
#endif
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




