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
			glm::vec2 screenCoordinates = ConvertToScreenCoordinates(transform->WorldPosition);
			renderer2D->RenderRectangle.x = screenCoordinates.x - transform->WorldScale.x / 2;
			renderer2D->RenderRectangle.y = screenCoordinates.y - transform->WorldScale.y / 2;
			renderer2D->RenderRectangle.w = transform->WorldScale.x;
			renderer2D->RenderRectangle.h = transform->WorldScale.y;

			SDL_SetTextureColorMod(renderer2D->RenderTexture,
				renderer2D->Color.r,
				renderer2D->Color.g,
				renderer2D->Color.b);

			SDL_SetTextureAlphaMod(renderer2D->RenderTexture,
				renderer2D->Color.a);

			SDL_RenderTextureRotated(Application::GetCoreInstance().GetMainRenderer(),
			                         renderer2D->RenderTexture, nullptr, &renderer2D->RenderRectangle,
			                         -glm::degrees(transform->WorldRotation), nullptr, SDL_FLIP_NONE);
		});

	//Render UI on top of sprites
	ECSManager::GetInstance().ForEachUsingComponents<Components::Transform, Components::UITexture>(
	[&](const Components::Transform* transform, Components::UITexture* uiTexture)
	{
		glm::vec2 screenCoordinates = ConvertToScreenCoordinates(transform->WorldPosition);
		uiTexture->RenderRectangle.x = screenCoordinates.x - transform->WorldScale.x / 2;
		uiTexture->RenderRectangle.y = screenCoordinates.y - transform->WorldScale.y / 2;
		uiTexture->RenderRectangle.w = transform->WorldScale.x;
		uiTexture->RenderRectangle.h = transform->WorldScale.y;

		SDL_RenderTextureRotated(Application::GetCoreInstance().GetMainRenderer(),
								 uiTexture->RenderTexture, nullptr, &uiTexture->RenderRectangle,
								 -glm::degrees(transform->WorldRotation), nullptr, SDL_FLIP_NONE);
	});
}

void Core::ECS::Systems::RenderingSystem::EndSystem()
{
}
