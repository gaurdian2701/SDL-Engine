#pragma once

namespace Components
{
	struct CircleCollider2D
	{
		CircleCollider2D() = default;
		explicit CircleCollider2D(float someRadius)
		{
			Radius = someRadius;
		}
		~CircleCollider2D() = default;

		float Radius = 1.0f;
		bool IsColliding = false;
	};
}