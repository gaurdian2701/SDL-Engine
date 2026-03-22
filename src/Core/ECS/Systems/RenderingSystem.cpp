#include "Core/ECS/Systems/RenderingSystem.h"
#include "Application/Application.h"
#include "Core/ECS/ECSManager.h"
#include "SDL3/SDL_render.h"
#include "Components/Renderer2D.h"
#include "Components/Transform.h"
#include "Components/UITexture.h"
#include "Core/HelperFunctions.h"

void Core::ECS::Systems::RenderingSystem::RegisterInterestedComponents()
{
	ECSManager::GetInstance().RegisterInterestedComponentsForSystem<Components::Transform,
		Components::Renderer2D, Components::UITexture>(this);
}

void Core::ECS::Systems::RenderingSystem::UpdateSystem(const float deltaTime)
{
	System::UpdateSystem(deltaTime);

	//Render regular 2D sprites
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::Renderer2D>(
		[&](const Components::Transform* transform, Components::Renderer2D* renderer2D)
		{
			glm::vec2 screenCoordinates = WorldToScreen(transform->Position);
			renderer2D->RenderRectangle.x = screenCoordinates.x - transform->Scale.x / 2;
			renderer2D->RenderRectangle.y = screenCoordinates.y - transform->Scale.y / 2;
			renderer2D->RenderRectangle.w = transform->Scale.x;
			renderer2D->RenderRectangle.h = transform->Scale.y;

			SDL_SetTextureColorMod(renderer2D->RenderTexture,
				renderer2D->Color.r,
				renderer2D->Color.g,
				renderer2D->Color.b);

			SDL_SetTextureAlphaMod(renderer2D->RenderTexture,
				renderer2D->Color.a);

			SDL_RenderTextureRotated(Application::GetCoreInstance().GetMainRenderer(),
			                         renderer2D->RenderTexture, nullptr, &renderer2D->RenderRectangle,
			                         -glm::degrees(transform->Rotation), nullptr, SDL_FLIP_NONE);
		});

	//Render UI on top of sprites
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::UITexture>(
	[&](const Components::Transform* transform, Components::UITexture* uiTexture)
	{
		glm::vec2 screenCoordinates = WorldToScreen(transform->Position);
		uiTexture->RenderRectangle.x = screenCoordinates.x - transform->Scale.x / 2;
		uiTexture->RenderRectangle.y = screenCoordinates.y - transform->Scale.y / 2;
		uiTexture->RenderRectangle.w = transform->Scale.x;
		uiTexture->RenderRectangle.h = transform->Scale.y;

		SDL_RenderTextureRotated(Application::GetCoreInstance().GetMainRenderer(),
								 uiTexture->RenderTexture, nullptr, &uiTexture->RenderRectangle,
								 -glm::degrees(transform->Rotation), nullptr, SDL_FLIP_NONE);
	});
}

void Core::ECS::Systems::RenderingSystem::EndSystem()
{
}
