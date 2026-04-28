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
                if (!isStatic && mass - someMass > 0.0f)
                {
                    MassDirtyByte = true;
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

        void SetMomentOfInertia(float someMomentOfInertia)
        {
            if (someMomentOfInertia > 0.0f)
            {
                if (!isStatic)
                {
                    momentOfInertia = someMomentOfInertia;
                    inverseMomentOfInertia = 1 / someMomentOfInertia;
                }
            }
            else
            {
                momentOfInertia = 0.0f;
                inverseMomentOfInertia = 0.0f;
            }
        }

        void SetIsStatic(bool staticBool)
        {
            if (staticBool)
            {
                mass = 0.0f;
                inverseMass = 0.0f;
                momentOfInertia = 0.0f;
                inverseMomentOfInertia = 0.0f;
            }
            else
            {
                mass = 1.0f;
                inverseMass = 1.0f;
                momentOfInertia = 1.0f;
                inverseMomentOfInertia = 1.0f;
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

        float GetMomentOfInertia() const
        {
            return momentOfInertia;
        }

        float GetInverseMomentOfInertia() const
        {
            return inverseMomentOfInertia;
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
        float AngularVelocity = 0.0f;
        float StaticFriction = 0.6f;
        float DynamicFriction = 0.3f;
        bool MassDirtyByte = true;

    private:
        float mass = 1.0f;
        float inverseMass = 1.0f;
        float momentOfInertia = 0.0f;
        float inverseMomentOfInertia = 0.0f;
        bool isStatic = false;
    };
}