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
                if (!isStatic)
                {
                    mass = someMass;
                    inverseMass = 1.0f / someMass;
                }
            }
            else
            {
                mass = 0.0f;
                inverseMass = 0.0f;
            }
        }

        void SetIsStatic(bool staticBool)
        {
            if (staticBool)
            {
                mass = 0.0f;
                inverseMass = 0.0f;
            }
            else
            {
                mass = 1.0f;
                inverseMass = 1.0f;
            }
            isStatic = staticBool;
        }

        float GetMass() const
        {
            return mass;
        }

        float GetInverseMass() const
        {
            return inverseMass;
        }

        bool IsStatic()
        {
            return isStatic;
        }

        void AddForce(const glm::vec2& someForce)
        {
            LinearAcceleration += inverseMass * someForce;
        }

    public:
        glm::vec2 LinearVelocity = glm::vec2(0.0f);
        glm::vec2 LinearAcceleration = glm::vec2(0.0f);
        float Restitution = 0.1f;

    private:
        float momentOfInertia = 0.0f;
        float mass = 1.0f;
        float inverseMass = 1.0f;
        bool isStatic = false;
    };
}