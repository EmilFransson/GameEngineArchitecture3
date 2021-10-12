#include "pch.h"
#include "ResourceManager.h"

ResourceManager ResourceManager::s_Instance;

void ResourceManager::MapPackageContent() noexcept
{
	for (const auto& file : std::filesystem::directory_iterator("packages/"))
	{
		std::cout << file.path() << "\n";
	}
}
