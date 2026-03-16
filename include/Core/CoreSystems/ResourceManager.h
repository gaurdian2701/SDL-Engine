#pragma once
#include <string>
#include <unordered_map>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include "SDL3_ttf/SDL_ttf.h"

namespace Core::CoreSystems
{
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager();

		SDL_Texture* TryLoadAndGetTexture(const std::string& someTextureFilePath);
		SDL_Texture* GetTexture(const std::string& someImageFilePath);
		TTF_Font* TryLoadAndGetFont(const std::string& someFontFilePath, float fontSize);
		TTF_Font* GetFont(const std::string& someFontFilePath);
		void ClearData();

	private:
		std::unordered_map<std::string, SDL_Texture*> m_textureMap = std::unordered_map<std::string, SDL_Texture*>();
		std::unordered_map<std::string, TTF_Font*> m_fontMap = std::unordered_map<std::string, TTF_Font*>();
	};
}
