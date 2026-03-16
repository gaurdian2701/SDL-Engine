#pragma once
#include <string>
#include "Application/Application.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

namespace Assets::Components
{
	struct UITexture
	{
		//Default Constructor
		UITexture() = default;
		~UITexture()
		{
			TextFont = nullptr;
			RenderTexture = nullptr;
		}

		//Copy Constructor
		UITexture(const UITexture& other)
		{
			RenderRectangle = other.RenderRectangle;
			RenderTexture = other.RenderTexture;
			TextFont = other.TextFont;
		}

		//Move Constructor
		UITexture(UITexture&& other) noexcept
		{
			RenderRectangle = other.RenderRectangle;
			RenderTexture = other.RenderTexture;
			TextFont = other.TextFont;
		}

		//Copy Assignment Operator
		UITexture& operator=(const UITexture& other)
		{
			if (this != &other)
			{
				RenderRectangle = other.RenderRectangle;
				RenderTexture = other.RenderTexture;
				TextFont = other.TextFont;
			}
			return *this;
		}

		//Move Assignment Operator
		UITexture& operator=(UITexture&& other) noexcept
		{
			if (this != &other)
			{
				RenderRectangle = other.RenderRectangle;
				RenderTexture = other.RenderTexture;
				TextFont = other.TextFont;
			}
			return *this;
		}

		void SetFontText(const std::string& someText, SDL_Color&& someColor)
		{
			SDL_Surface* surface = TTF_RenderText_Solid(TextFont, someText.c_str(), 0, someColor);
			RenderTexture = SDL_CreateTextureFromSurface(Application::GetCoreInstance().GetMainRenderer(), surface);
			SDL_DestroySurface(surface);
		}

		SDL_FRect RenderRectangle = SDL_FRect(0, 0, 5, 5);
		TTF_Font* TextFont = nullptr;
		SDL_Texture* RenderTexture = nullptr;
	};
}
