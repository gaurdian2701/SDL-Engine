#pragma once

namespace Components
{
    struct CircleCollider2D
    {
        CircleCollider2D() = default;
        ~CircleCollider2D() = default;

        //Copy Constructor
        CircleCollider2D(const CircleCollider2D& other)
        {
            Radius = other.Radius;
        }

        //Move Constructor
        CircleCollider2D(CircleCollider2D&& other) noexcept
        {
            if (this != &other)
            {
                Radius = other.Radius;
            }
        }

        //Copy Assignment Operator
        CircleCollider2D& operator=(const CircleCollider2D& other)
        {
            if (this != &other)
            {
                Radius = other.Radius;
            }

            return *this;
        }

        //Move Assignment Operator
        CircleCollider2D& operator=(CircleCollider2D&& other) noexcept
        {
            if (this != &other)
            {
                Radius = other.Radius;
            }

            return *this;
        }

        float Radius = 1.0f;
        bool IsColliding = false;
    };
}