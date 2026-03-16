#pragma once
#include "vec2.hpp"
#include "Application/Application.h"

namespace Core
{
    static inline glm::vec2 ConvertToScreenCoordinates(glm::vec2 someCartesianCoordinates)
    {
        return glm::vec2(someCartesianCoordinates.x + Application::SCREEN_WIDTH * 0.5f,
            Application::SCREEN_HEIGHT * 0.5f - someCartesianCoordinates.y);
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
