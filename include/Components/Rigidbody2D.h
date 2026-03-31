#pragma once

namespace Components
{
    struct Rigidbody2D
    {
        //Default Constructor
        Rigidbody2D() = default;
        ~Rigidbody2D() = default;

        void SetMass(float someMass)
        {
            if (someMass > 0.0f)
            {
                mass = someMass;
                inverseMass = 1.0f / someMass;
            }
            else
            {
                mass = 0.0f;
                inverseMass = 0.0f;
            }
        }

        float GetMass() const
        {
            return mass;
        }

        float GetInverseMass() const
        {
            return inverseMass;
        }

        glm::vec2 Velocity = glm::vec2(0.0f);
        float Restitution = 1.0f;

    private:
        float mass = 1.0f;
        float inverseMass = 1.0f;
    };
}