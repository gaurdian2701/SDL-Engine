#pragma once
#include "vec2.hpp"
#include "Core/ECS/ECSData.h"

enum CollisionType : std::uint8_t
{
	CircleVsCircle = 0,
	PolygonVsCircle = 1,
	PolygonVsPolygon = 2,
};

struct CollisionPair
{
	std::uint32_t EntityA = Core::INVALID_ENTITY_ID;
	std::uint32_t EntityB = Core::INVALID_ENTITY_ID;
	CollisionType Type = PolygonVsPolygon;
};
