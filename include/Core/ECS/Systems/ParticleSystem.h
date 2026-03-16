#pragma once
#include <random>

#include "System.h"

namespace Assets::Components
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
		void BeginSystem() override;
        void ProcessInitializationQueue() override;
		void UpdateSystem(const float deltaTime) override;
		void EndSystem() override{}

	private:
		inline void RenderParticle(Assets::Components::ParticleEmitter* someParticleEmitter,
			Assets::Components::Particle& someParticle);

	private:
		std::default_random_engine m_randomOffsetGenerator;
		glm::vec2 m_minCartesianLimits = glm::vec2(0.0f, 0.0f);
		glm::vec2 m_maxCartesianLimits = glm::vec2(0.0f, 0.0f);
	};
}
