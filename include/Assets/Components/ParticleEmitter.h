#pragma once
#include <vector>
#include "SDL3/SDL_render.h"
#include "glm.hpp"

namespace Scene
{
	class GameObject;
}

namespace Assets::Components
{
	struct Particle
	{
		Particle() = default;
		~Particle()
		{

		}
		//Copy Constructor
		Particle(const Particle& other)
		{
			CurrentPosition = other.CurrentPosition;
			CurrentLifeTime = other.CurrentLifeTime;
			ParticleSize = other.ParticleSize;
		}

		//Move Constructor
		Particle(Particle&& other) noexcept
		{
			CurrentPosition = other.CurrentPosition;
			CurrentLifeTime = other.CurrentLifeTime;
			ParticleSize = other.ParticleSize;
		}

		//Copy Assignment Operator
		Particle& operator=(const Particle& other)
		{
			if (this != &other)
			{
				CurrentPosition = other.CurrentPosition;
				CurrentLifeTime = other.CurrentLifeTime;
				ParticleSize = other.ParticleSize;
			}
			return *this;
		}

		//Move Assignment operator
		Particle& operator=(Particle&& other) noexcept
		{
			if (this != &other)
			{
				CurrentPosition = other.CurrentPosition;
				CurrentLifeTime = other.CurrentLifeTime;
				ParticleSize = other.ParticleSize;
			}
			return *this;
		}

		glm::vec2 CurrentPosition = glm::vec2(0.0f);

		float CurrentLifeTime = 0.0f;
		float ParticleSize = 0.0f;
	};

	struct ParticleEmitter
	{
		ParticleEmitter() = default;

		~ParticleEmitter()
		{
			RenderTexture = nullptr;
		}

		void InitializeEmitter(uint8_t numberOfParticles,
			glm::vec2&& someStartingOffset,
			glm::vec2&& someInitialDirection,
			uint16_t someMaxDeviation,
			float someSpeed,
			float someLifetime,
			float someParticleSize)
		{
			Particles.resize(numberOfParticles);
			StartingOffset = someStartingOffset;
			Direction = someInitialDirection;
			ParticleSpeed = someSpeed;
			MaxDeviation = someMaxDeviation;
			ParticleLifetime = someLifetime;

			for (auto& particle : Particles)
			{
				particle.CurrentLifeTime = someLifetime;
				particle.ParticleSize = someParticleSize;
			}
		}

		//Copy Constructor
		ParticleEmitter(const ParticleEmitter& other)
		{
			StartingOffset = other.StartingOffset;
			MaxDeviation = other.MaxDeviation;
			ParticleLifetime = other.ParticleLifetime;
			Owner = other.Owner;

			Color = other.Color;
			RenderRectangle = other.RenderRectangle;
			RenderTexture = other.RenderTexture;
			Particles.assign(other.Particles.begin(), other.Particles.end());
		}

		//Move Constructor
		ParticleEmitter(ParticleEmitter&& other) noexcept
		{
			StartingOffset = other.StartingOffset;
			MaxDeviation = other.MaxDeviation;
			ParticleLifetime = other.ParticleLifetime;
			Owner = other.Owner;

			Color = other.Color;
			RenderRectangle = other.RenderRectangle;
			RenderTexture = other.RenderTexture;
			Particles.assign(other.Particles.begin(), other.Particles.end());

			other.RenderTexture = nullptr;
			other.Particles.clear();
		}

		//Copy Assignment operator
		ParticleEmitter& operator=(const ParticleEmitter& other)
		{
			if (this != &other)
			{
				StartingOffset = other.StartingOffset;
				MaxDeviation = other.MaxDeviation;
				ParticleLifetime = other.ParticleLifetime;
				Owner = other.Owner;

				Color = other.Color;
				RenderRectangle = other.RenderRectangle;
				RenderTexture = other.RenderTexture;
				Particles.assign(other.Particles.begin(), other.Particles.end());
			}
			return *this;
		}

		//Move Assignment operator
		ParticleEmitter& operator=(ParticleEmitter&& other) noexcept
		{
			if (this != &other)
			{
				StartingOffset = other.StartingOffset;
				MaxDeviation = other.MaxDeviation;
				ParticleLifetime = other.ParticleLifetime;
				Owner = other.Owner;

				Color = other.Color;
				RenderRectangle = other.RenderRectangle;
				RenderTexture = other.RenderTexture;
				Particles.assign(other.Particles.begin(), other.Particles.end());

				other.RenderTexture = nullptr;
				other.Particles.clear();
			}
			return *this;
		}

		glm::vec2 StartingOffset = glm::vec2(0.0f);
		glm::vec2 Direction = glm::vec2(0.0f);

		uint16_t MaxDeviation = 1;

		float ParticleLifetime = 0.1f;
		float ParticleSpeed = 0.0f;
		bool UseOwnerDirection = false;

		SDL_FColor Color = SDL_FColor(255, 255, 255, 255);
		SDL_FRect RenderRectangle = SDL_FRect(0, 0, 5, 5);
		SDL_Texture* RenderTexture = nullptr;

		Scene::GameObject* Owner = nullptr;
		std::vector<Particle> Particles = std::vector<Particle>(5);
	};
}
