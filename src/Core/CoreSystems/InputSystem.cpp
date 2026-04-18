#include "Core/CoreSystems/InputSystem.h"

#include "DebugStatements.h"
#include "imgui_impl_sdl3.h"
#include "Application/Application.h"
#include "SDL3/SDL_events.h"

void Core::Input::InputSystem::Update()
{
    // Reset per-frame pressed/released states
    m_pressedThisFrame.fill(false);
    m_releasedThisFrame.fill(false);

    // Store previous state
    if (m_currentKeyState)
    {
        for (int i = 0; i < SDL_SCANCODE_COUNT; i++)
        {
            m_previousKeyState[i] = m_currentKeyState[i];
        }
    }

    // Update current state
    SDL_PumpEvents();
    m_currentKeyState = SDL_GetKeyboardState(nullptr);

    // Process events (for precise key press detection)
    ProcessEvents();

    // Update mouse
    float x, y;
    SDL_GetMouseState(&x, &y);
    m_mouseScreenCoords = glm::vec2(x, y);
}

void Core::Input::InputSystem::ProcessEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
        {
            Application::GetCoreInstance().StopRunning();
        }

        DoDebugCode(
        ImGui_ImplSDL3_ProcessEvent(&event);
        );

        if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat)
        {
            m_pressedThisFrame[event.key.scancode] = true;
        }
        else if (event.type == SDL_EVENT_KEY_UP)
        {
            m_releasedThisFrame[event.key.scancode] = true;
        }
    }
}

bool Core::Input::InputSystem::IsKeyDown(SDL_Scancode key) const
{
    return m_currentKeyState && m_currentKeyState[key];
}

bool Core::Input::InputSystem::IsKeyPressed(SDL_Scancode key) const
{
    return m_pressedThisFrame[key];
}

bool Core::Input::InputSystem::IsKeyReleased(SDL_Scancode key) const
{
    return m_releasedThisFrame[key];
}

glm::vec2 Core::Input::InputSystem::GetMouseScreenCoords() const
{
    return m_mouseScreenCoords;
}