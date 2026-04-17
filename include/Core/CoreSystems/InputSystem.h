#pragma once
#include "vec2.hpp"
#include "Core/CoreSystems/CoreSystem.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keyboard.h"

namespace Core::Input
{
    class InputSystem : public CoreSystem
    {
    private:
        InputSystem() = default;
        ~InputSystem() override = default;

        void CatchEvents();

    public:
        static InputSystem& GetInstance()
        {
            static InputSystem* instance = new InputSystem();
            return *instance;
        }

        void Update() override;
        [[nodiscard]] bool CheckForKeyPress(SDL_Keycode someKeyCode);
        [[nodiscard]] const glm::vec2 GetMouseScreenCoords() const;

    private:
        glm::vec2 m_mouseScreenCoords = glm::vec2(0.f);
        float m_timeToRegisterInput = 0.5f;
        float m_inputTimer = 0.0f;
        const bool* m_currentKeyBoardState = SDL_GetKeyboardState(nullptr);
        bool m_inputBeingRegistered = true;
    };
}
