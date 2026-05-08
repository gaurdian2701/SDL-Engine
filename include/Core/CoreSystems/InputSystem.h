#pragma once
#include "vec2.hpp"
#include "Core/CoreSystems/CoreSystem.h"
#include "SDL3/SDL_events.h"
#include <array>

namespace Core::Input
{
    class InputSystem : public CoreSystem
    {
    private:
        InputSystem() = default;
    public:
        ~InputSystem() override = default;

        void ProcessEvents();

    public:
        static InputSystem& GetInstance()
        {
            static InputSystem instance;
            return instance;
        }

        void Update() override;

        // Key queries
        [[nodiscard]] bool IsKeyDown(SDL_Scancode key) const;
        [[nodiscard]] bool IsKeyPressed(SDL_Scancode key) const;
        [[nodiscard]] bool IsKeyReleased(SDL_Scancode key) const;

        // Mouse
        [[nodiscard]] glm::vec2 GetMouseScreenCoords() const;

    private:
        const bool* m_currentKeyState = nullptr;
        std::array<std::uint8_t, SDL_SCANCODE_COUNT> m_previousKeyState{};

        std::array<bool, SDL_SCANCODE_COUNT> m_pressedThisFrame{};
        std::array<bool, SDL_SCANCODE_COUNT> m_releasedThisFrame{};

        glm::vec2 m_mouseScreenCoords = glm::vec2(0.f);
    };
}
