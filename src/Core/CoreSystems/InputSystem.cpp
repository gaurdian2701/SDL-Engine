#include "Core/CoreSystems/InputSystem.h"
#include "SDL3/SDL_events.h"

void Core::Input::InputSystem::Update()
{
    SDL_PumpEvents();
}

bool Core::Input::InputSystem::CheckForKeyPress(const SDL_Keycode someKeyCode) const
{
    return m_currentKeyBoardState[someKeyCode];
}

