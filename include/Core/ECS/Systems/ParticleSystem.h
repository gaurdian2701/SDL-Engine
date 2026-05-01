#pragma once
#include <random>

#include "System.h"

namespace Components
{
	struct Particle;
	struct ParticleEmitter;
}

namespace Core::ECS::Systems
{
	class ParticleSystem : public System
	{
	public:
		ParticleSystem() = default;
		~ParticleSystem() override = default;

		void RegisterInterestedComponents() override;
		void OnComponentAdded(const std::uint32_t entityID) override;
		void UpdateSystem(const float deltaTime) override;
		void EndSystem() override{}

	private:
		inline void RenderParticle(Components::ParticleEmitter* someParticleEmitter,
			Components::Particle& someParticle);

	private:
		std::default_random_engine m_randomOffsetGenerator;
	};
}
