#pragma once

namespace Core::ECS::Systems::Physics
{
	class NarrowPhase
	{
	public:
		NarrowPhase() = default;
		~NarrowPhase() = default;

		void DoNarrowPhase(const float deltaTime);

	private:
		void SolveCircleVsCircle();
		void SolveAABBVsAABB();
	};
}
