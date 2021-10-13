#include "pch.h"
#include "ResourceManager.h"
#include "PackageTool.h"
#include "OBJ_Loader.h"

ResourceManager ResourceManager::s_Instance;

template<>
std::shared_ptr<Texture2D> ResourceManager::Load(const std::string& filePath) noexcept
{
	if (m_Map.contains(filePath))
	{
		return dynamic_pointer_cast<Texture2D>(m_Map[filePath]);
	}
	else
	{
		std::ifstream pkg("Packages/" + m_PackageFileMap[filePath], std::ios::binary);
		if (pkg.is_open())
		{
			PackageTool::PackageHeader pkghdr{};
			pkg.read((char*)&pkghdr, sizeof(PackageTool::PackageHeader));
			bool foundTexture = false;
			for (uint32_t i{ 0u }; i < pkghdr.assetCount && foundTexture == false; ++i)
			{
				PackageTool::ChunkHeader chHdr{};
				pkg.read((char*)&chHdr, sizeof(PackageTool::ChunkHeader));
				std::unique_ptr<char> fileName = std::unique_ptr<char>(DBG_NEW char[chHdr.readableSize + 1]);
				pkg.read(fileName.get(), chHdr.readableSize);
				fileName.get()[chHdr.readableSize] = '\0';
				if (strcmp(fileName.get(), filePath.data()) != 0)
				{
					pkg.seekg(chHdr.chunkSize, std::ios_base::cur);
				}
				else
				{
					foundTexture = true;
				}
			}
			if (foundTexture == true)
			{
				PackageTool::TextureHeader texHdr{};
				pkg.read((char*)&texHdr, sizeof(PackageTool::TextureHeader));
				std::unique_ptr<char> textureBuffer = std::unique_ptr<char>(DBG_NEW char[texHdr.dataSize]);
				pkg.read(textureBuffer.get(), texHdr.dataSize);
				pkg.close();

				std::shared_ptr<Texture2D> resourceType = std::make_shared<Texture2D>(texHdr.width, texHdr.height, texHdr.rowPitch, textureBuffer.get());

				std::shared_ptr<Resource> resource = dynamic_pointer_cast<Resource>(resourceType);
				m_Map[filePath] = resource;

				return resourceType;
			}
			else
			{
				return nullptr;
			}
		}
		return nullptr;
	}
}

template<>
std::shared_ptr<MeshOBJ> ResourceManager::Load(const std::string& filePath) noexcept
{
	if (m_Map.contains(filePath))
	{
		return dynamic_pointer_cast<MeshOBJ>(m_Map[filePath]);
	}
	else
	{
		std::ifstream pkg("Packages/" + m_PackageFileMap[filePath], std::ios::binary);
		if (pkg.is_open())
		{
			PackageTool::PackageHeader pkghdr{};
			pkg.read((char*)&pkghdr, sizeof(PackageTool::PackageHeader));
			bool foundOBJ = false;
			for (uint32_t i{ 0u }; i < pkghdr.assetCount && foundOBJ == false; ++i)
			{
				PackageTool::ChunkHeader chkHdr{};
				pkg.read((char*)&chkHdr, sizeof(PackageTool::ChunkHeader));
				if (memcmp(chkHdr.type, "MESH", 4) == 0)
				{
					std::unique_ptr<char> pFileName = std::unique_ptr<char>(DBG_NEW char[chkHdr.readableSize + 1]);
					pkg.read(pFileName.get(), chkHdr.readableSize);
					pFileName.get()[chkHdr.readableSize] = '\0';
					if (strcmp(pFileName.get(), filePath.c_str()) == 0)
					{
						foundOBJ = true;
					}
					else
					{
						if (i != pkghdr.assetCount - 1u)
						{
							pkg.seekg(chkHdr.chunkSize, std::ios_base::cur);
						}
					}
				}
			}
			if (foundOBJ)
			{
				PackageTool::MeshHeader meshHdr{};
				pkg.read((char*)&meshHdr, sizeof(PackageTool::MeshHeader));
				std::vector<objl::Vertex> vertices;
				vertices.resize(meshHdr.verticesDataSize / sizeof(objl::Vertex));
				pkg.read((char*)vertices.data(), meshHdr.verticesDataSize);
				std::vector<unsigned int> indices;
				indices.resize(meshHdr.indicesDataSize / sizeof(unsigned int));
				pkg.read((char*)indices.data(), meshHdr.indicesDataSize);
				pkg.close();

				std::shared_ptr<MeshOBJ> resourceType = std::make_shared<MeshOBJ>(vertices, indices);

				m_Map[filePath] = dynamic_pointer_cast<Resource>(resourceType);
				return resourceType;
			}
			pkg.close();
		}
	
	}
	return nullptr;
}

void ResourceManager::MapPackageContent() noexcept
{
	std::vector<std::filesystem::directory_entry> packages;
	for (const auto& package : std::filesystem::directory_iterator("Packages/"))
	{
		std::string path = package.path().filename().string();
		
		if (m_PackageFileMap.contains(path))
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
				
				m_PackageFileMap[fileName.get()] = path;
				
				if (i != pkgHdr.assetCount - 1u)
					inFile.seekg(chkHdr.chunkSize, std::ios_base::cur);
			}
			inFile.close();
		}
	}
}
