#include "Core/ECS/Systems/ParticleSystem.h"
#include "Core/HelperFunctions.h"
#include "Assets/Components/ParticleEmitter.h"
#include "Assets/Components/Transform.h"
#include "Core/ECS/ECSManager.h"

void Core::ECS::Systems::ParticleSystem::RegisterInterestedComponents()
{
	ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Assets::Components::Transform,
		Assets::Components::ParticleEmitter>(this);
}

void Core::ECS::Systems::ParticleSystem::BeginSystem()
{
	m_maxCartesianLimits = GetMaxCartesianLimits();
	m_minCartesianLimits = GetMinCartesianLimits();
}

void Core::ECS::Systems::ParticleSystem::ProcessInitializationQueue()
{
	ECSManager::GetInstance().ForEachUsingEntities<Assets::Components::Transform, Assets::Components::ParticleEmitter>
	(m_initializationQueue,
	 [&](const Assets::Components::Transform *transform, Assets::Components::ParticleEmitter *particleEmitter)
	 {
		 std::uniform_int_distribution<int> randomDistribution(
			 -particleEmitter->MaxDeviation, particleEmitter->MaxDeviation);

		 for (auto &particle: particleEmitter->Particles) {
			 //Simulation
			 //Set particle's initial position according to the initial velocity.
			 //The positions are relative to the particle emitter's world position
			 particle.CurrentPosition = transform->WorldPosition + particleEmitter->StartingOffset +
			                            glm::vec2(randomDistribution(m_randomOffsetGenerator),
			                                      randomDistribution(m_randomOffsetGenerator));

			 //Rendering
			 RenderParticle(particleEmitter, particle);
		 }
	 });
}

void Core::ECS::Systems::ParticleSystem::UpdateSystem(const float deltaTime)
{
	System::UpdateSystem(deltaTime);

	glm::vec2 particleVelocity = glm::vec2(0.0f);

	ECSManager::GetInstance().ForEachUsingComponents<Assets::Components::Transform,
		Assets::Components::ParticleEmitter>(
		[&, this](Assets::Components::Transform *transform, Assets::Components::ParticleEmitter *particleEmitter)
		{
			std::uniform_int_distribution<int> randomDistribution(
				-particleEmitter->MaxDeviation, particleEmitter->MaxDeviation);

			if (particleEmitter->UseOwnerDirection)
			{
				particleVelocity = particleEmitter->ParticleSpeed * transform->Up;
			}
			else
			{
				particleVelocity = particleEmitter->ParticleSpeed * particleEmitter->Direction;
			}

			for (auto &particle: particleEmitter->Particles)
			{
				//Simulation
				particle.CurrentPosition += particleVelocity * deltaTime;

				if (particle.CurrentLifeTime < 0.0f) {
					//Reset particle position next to particle emitter
					particle.CurrentPosition = transform->WorldPosition + particleEmitter->StartingOffset +
					                           glm::vec2(randomDistribution(m_randomOffsetGenerator),
					                                     randomDistribution(m_randomOffsetGenerator));

					particle.CurrentLifeTime = particleEmitter->ParticleLifetime;
				}

				particle.CurrentLifeTime -= deltaTime;

				//Rendering
				this->RenderParticle(particleEmitter, particle);
			}
		});
}

inline void Core::ECS::Systems::ParticleSystem::RenderParticle(Assets::Components::ParticleEmitter *someParticleEmitter,
                                                               Assets::Components::Particle &someParticle)
{
	glm::vec2 screenCoordinates = ConvertToScreenCoordinates(someParticle.CurrentPosition);
	someParticleEmitter->RenderRectangle.x = screenCoordinates.x - someParticle.ParticleSize * 0.5f;
	someParticleEmitter->RenderRectangle.y = screenCoordinates.y - someParticle.ParticleSize * 0.5f;
	someParticleEmitter->RenderRectangle.w = someParticle.ParticleSize;
	someParticleEmitter->RenderRectangle.h = someParticle.ParticleSize;

	SDL_SetTextureColorMod(someParticleEmitter->RenderTexture,
	                       someParticleEmitter->Color.r,
	                       someParticleEmitter->Color.g,
	                       someParticleEmitter->Color.b);

	SDL_RenderTexture(Application::GetCoreInstance().GetMainRenderer(),
	                  someParticleEmitter->RenderTexture,
	                  nullptr,
	                  &someParticleEmitter->RenderRectangle);
}
