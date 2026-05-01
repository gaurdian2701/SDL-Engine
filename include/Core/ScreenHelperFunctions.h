#pragma once
#include "vec2.hpp"
#include "Application/Application.h"

namespace Core::ScreenHelperFunctions
{
    static inline glm::vec2 WorldToScreen(const glm::vec2 someCartesianCoordinates)
    {
        static Application* appInstance = &Application::GetCoreInstance();
        return glm::vec2(someCartesianCoordinates.x + appInstance->GetScreenWidth() * 0.5f,
            appInstance->GetScreenHeight() * 0.5f - someCartesianCoordinates.y);
    }

    static inline glm::vec2 ScreenToWorld(const glm::vec2 someScreenCoordinates)
    {
        static Application* appInstance = &Application::GetCoreInstance();
        return glm::vec2(someScreenCoordinates.x - appInstance->GetScreenWidth() * 0.5f,
            appInstance->GetScreenHeight() * 0.5f - someScreenCoordinates.y);
    }
}
