#pragma once
#include <filesystem>
#include <iostream>
#include <string>

#include "PrintDebug.h"

namespace Core
{
	class AssetPathHolder
	{
	private:
		AssetPathHolder() = default;
		~AssetPathHolder() = default;

	public:
		static AssetPathHolder& GetInstance()
		{
			static AssetPathHolder* instance = new AssetPathHolder();
			return *instance;
		}

		void UpdateAssetFolderPath(const std::string someAssetFolderPath)
		{
			m_assetFolderPath = someAssetFolderPath;
		}

		std::string GetAssetPath(const std::string someAssetFilePath)
		{
			std::filesystem::path fullPath = std::filesystem::current_path() / m_assetFolderPath / someAssetFilePath;
			fullPath = fullPath.lexically_normal();
			PrintDebug("%s\n", fullPath.string().c_str());
			return fullPath.string();
		}

	private:
		std::filesystem::path m_assetFolderPath = "";
	};
}
