#include "Core/CoreSystems/ResourceManager.h"
#include "Application/Application.h"
#include "Core/AssetPathHolder.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3_ttf/SDL_ttf.h"

Core::CoreSystems::ResourceManager::~ResourceManager()
{
	ClearData();
}

SDL_Texture* Core::CoreSystems::ResourceManager::TryLoadAndGetTexture(const std::string& someTextureFilePath)
{
	if (!m_textureMap.contains(someTextureFilePath))
	{
		SDL_Surface* surface = SDL_LoadPNG(
			AssetPathHolder::GetInstance().GetAssetPath(someTextureFilePath).c_str());
		m_textureMap[someTextureFilePath] = SDL_CreateTextureFromSurface(
			Application::GetCoreInstance().GetMainRenderer(),
			surface);
		SDL_DestroySurface(surface);
	}

	return m_textureMap[someTextureFilePath];
}

TTF_Font* Core::CoreSystems::ResourceManager::TryLoadAndGetFont(const std::string& someFontFilePath, float fontSize)
{
	if (!m_fontMap.contains(someFontFilePath))
	{
		TTF_Font *font = TTF_OpenFont(AssetPathHolder::GetInstance().GetAssetPath(someFontFilePath).c_str(), fontSize);
		m_fontMap[someFontFilePath] = font;
	}

	return m_fontMap[someFontFilePath];
}

TTF_Font* Core::CoreSystems::ResourceManager::GetFont(const std::string& someFontFilePath)
{
	return m_fontMap[someFontFilePath];
}

SDL_Texture* Core::CoreSystems::ResourceManager::GetTexture(const std::string& someImageFilePath)
{
	return m_textureMap[someImageFilePath];
}

void Core::CoreSystems::ResourceManager::ClearData()
{
	for (const auto& texture : m_textureMap)
	{
		SDL_DestroyTexture(texture.second);
	}
	for (const auto& font : m_fontMap)
	{
		TTF_CloseFont(font.second);
	}

	m_textureMap.clear();
	m_fontMap.clear();
}




