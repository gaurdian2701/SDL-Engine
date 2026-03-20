#pragma once
#include "vec2.hpp"
#include "Core/ECS/ECSData.h"

struct CollisionPair
{
	std::uint32_t EntityA = Core::INVALID_ENTITY_ID;
	std::uint32_t EntityB = Core::INVALID_ENTITY_ID;
};

struct ContactPoint
{
	glm::vec2 Point;
	float Penetration;
};
