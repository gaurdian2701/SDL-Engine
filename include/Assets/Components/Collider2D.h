#pragma once
#include "vec2.hpp"
#include "Core/ECS/ECSData.h"

namespace Assets::Components
{
	struct Collider2D
	{
		Collider2D() = default;
		~Collider2D() = default;

		//Copy Constructor
		Collider2D(const Collider2D& other)
		{
			HalfSize = other.HalfSize;
			EntityCollidedWith = other.EntityCollidedWith;
		}

		//Move Constructor
		Collider2D(Collider2D&& other) noexcept
		{
			if (this != &other)
			{
				HalfSize = other.HalfSize;
				EntityCollidedWith = other.EntityCollidedWith;
			}
		}

		//Copy Assignment Operator
		Collider2D& operator=(const Collider2D& other)
		{
			if (this != &other)
			{
				HalfSize = other.HalfSize;
				EntityCollidedWith = other.EntityCollidedWith;
			}
			return *this;
		}

		//Move Assignment Operator
		Collider2D& operator=(Collider2D&& other) noexcept
		{
			if (this != &other)
			{
				HalfSize = other.HalfSize;
				EntityCollidedWith = other.EntityCollidedWith;
			}
			return *this;
		}

		bool IsColliding() const
		{
			return EntityCollidedWith != Core::INVALID_ENTITY_ID;
		}

		glm::vec2 HalfSize = glm::vec2(0.0f);
		uint32_t EntityCollidedWith = Core::INVALID_ENTITY_ID;
	};

}
