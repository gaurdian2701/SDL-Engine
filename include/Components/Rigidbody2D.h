#pragma once

namespace Components
{
    struct Rigidbody2D
    {
        //Default Constructor
        Rigidbody2D() = default;
        ~Rigidbody2D() = default;

        //Copy Constructor
        Rigidbody2D(const Rigidbody2D& other)
        {

        }

        //Move Constructor
        Rigidbody2D(Rigidbody2D&& other) noexcept
        {
            if (this != &other)
            {
            }
        }

        //Copy Assignment Operator
        Rigidbody2D& operator=(const Rigidbody2D& other)
        {
            if (this != &other)
            {
            }

            return *this;
        }

        //Move Assignment Operator
        Rigidbody2D& operator=(Rigidbody2D&& other) noexcept
        {
            if (this != &other)
            {
            }

            return *this;
        }
    };
}