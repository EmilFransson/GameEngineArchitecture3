#include "pch.h"
#include "ResourceManager.h"

ResourceManager ResourceManager::s_Instance;

void ResourceManager::MapPackageContent() noexcept
{
	std::vector<std::filesystem::directory_entry> packages;
	std::map<std::string_view, std::string_view> packageFileMap;
	for (const auto& package : std::filesystem::directory_iterator("Packages/"))
	{
		std::string path = package.path().string();
		path = path.substr(path.find_last_of("/") + 1, path.size() - 1);
		
		if (packageFileMap.contains(path))
		{
			//Package already dealt with.
			continue;
		}

		std::ifstream inFile("Packages/" + path, std::ios::binary);
		if (inFile.is_open())
		{
			PackageTool::PackageHeader pkgHdr{};
			inFile.read((char*)&pkgHdr, sizeof(PackageTool::PackageHeader));
			for (uint32_t i{ 0u }; i < pkgHdr.assetCount; ++i)
			{
				PackageTool::ChunkHeader chkHdr{};
				inFile.read((char*)&chkHdr, sizeof(PackageTool::ChunkHeader));
				std::unique_ptr<char> fileName = std::unique_ptr<char>(DBG_NEW char[chkHdr.readableSize + 1]);
				inFile.read(fileName.get(), chkHdr.readableSize);
				fileName.get()[chkHdr.readableSize] = '\0';
				std::string fileNameAsStr = fileName.get();
				fileNameAsStr = fileNameAsStr.substr(fileNameAsStr.find_last_of("'\'") + 1, fileNameAsStr.size() - 1);
				packageFileMap[fileNameAsStr] = path;

			}
			inFile.close();
		}
	}
}
