#pragma once
#include <vector>

namespace Core::ECS::Systems::Physics
{
	class BroadPhase
	{
	public:
		BroadPhase() = default;
		virtual ~BroadPhase() = default;
		virtual void DoBroadPhase() = 0;
	};
}
