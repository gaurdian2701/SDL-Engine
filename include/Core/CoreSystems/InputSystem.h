#pragma once
#include "Core/CoreSystems/CoreSystem.h"
#include "SDL3/SDL_keyboard.h"

namespace Core::Input
{
    class InputSystem : public CoreSystem
    {
    private:
        InputSystem() = default;
        ~InputSystem() override = default;

    public:
        static InputSystem& GetInstance()
        {
            static InputSystem* instance = new InputSystem();
            return *instance;
        }

        void Update() override;
        [[nodiscard]] bool CheckForKeyPress(SDL_Keycode someKeyCode) const;

    private:
        const bool* m_currentKeyBoardState = SDL_GetKeyboardState(nullptr);
    };
}
