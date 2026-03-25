#pragma once

namespace Components
{
    struct Rigidbody2D
    {
        //Default Constructor
        Rigidbody2D() = default;
        ~Rigidbody2D() = default;

        glm::vec2 Velocity = glm::vec2(0.0f);
    };
}