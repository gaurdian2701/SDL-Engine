#include "Core/CoreSystems/InputSystem.h"

#include "PrintDebug.h"
#include "Application/Application.h"
#include "SDL3/SDL_events.h"

void Core::Input::InputSystem::Update()
{
    SDL_PumpEvents();
    CatchEvents();
}

void Core::Input::InputSystem::CatchEvents()
{
    SDL_GetMouseState(&m_mouseScreenCoords.x, &m_mouseScreenCoords.y);
}

bool Core::Input::InputSystem::CheckForKeyPress(const SDL_Keycode someKeyCode)
{
    return m_currentKeyBoardState[someKeyCode];
}

const glm::vec2 Core::Input::InputSystem::GetMouseScreenCoords() const
{
    return m_mouseScreenCoords;
}
