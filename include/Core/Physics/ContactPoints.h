#pragma once
#include <cstdint>
#include <vector>
#include "vec2.hpp"

namespace Core::Physics
{
    struct ContactPoints
    {
        std::uint8_t NumberOfContactPoints = 0;
        std::vector<glm::vec2> Points = std::vector<glm::vec2>(2);
    };
}
