#pragma once
#include "vec2.hpp"
#include "Application/Application.h"

namespace Core
{
    static inline glm::vec2 WorldToScreen(const glm::vec2 someCartesianCoordinates)
    {
        return glm::vec2(someCartesianCoordinates.x + Application::SCREEN_WIDTH * 0.5f,
            Application::SCREEN_HEIGHT * 0.5f - someCartesianCoordinates.y);
    }

    static inline glm::vec2 ScreenToWorld(const glm::vec2 someScreenCoordinates)
    {
        return glm::vec2(someScreenCoordinates.x - Application::SCREEN_WIDTH * 0.5f,
            Application::SCREEN_HEIGHT * 0.5f - someScreenCoordinates.y);
    }

    static inline const glm::vec2& GetMaxCartesianLimits()
    {
        static glm::vec2 limits = glm::vec2(Application::SCREEN_WIDTH * 0.5f,
            Application::SCREEN_HEIGHT * 0.5f);
        return limits;
    }

    static inline const glm::vec2& GetMinCartesianLimits()
    {
        static glm::vec2 limits = glm::vec2(-Application::SCREEN_WIDTH * 0.5f,
            -Application::SCREEN_HEIGHT * 0.5f);
        return limits;
    }
}
