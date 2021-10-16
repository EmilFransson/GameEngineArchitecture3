#include "pch.h"
#include "ResourceManager.h"
#include "PackageTool.h"
#include "OBJ_Loader.h"

ResourceManager ResourceManager::s_Instance;

template<>
std::shared_ptr<Texture2D> ResourceManager::Load(const std::string& fileName) noexcept
{
	if (m_Map.contains(fileName))
	{
		return dynamic_pointer_cast<Texture2D>(m_Map[fileName]);
	}
	else
	{
		if (LoadResourceFromPackage(fileName))
		{
			return dynamic_pointer_cast<Texture2D>(m_Map[fileName]);
		}
		std::cout << "Error: Unable to load asset " << fileName << "\n";
		assert(false); //For now, just assert false.
	}
	return nullptr; //Should never be reached.
}

template<>
std::shared_ptr<MeshOBJ> ResourceManager::Load(const std::string& fileName) noexcept
{
	std::ifstream package("Packages/" + m_PackageFileMap[fileName], std::ios::binary);
	if (package.is_open())
	{
		PackageTool::PackageHeader packageHeader{};
		package.read((char*)&packageHeader, sizeof(PackageTool::PackageHeader));
		bool foundOBJ = false;
		for (uint32_t i{ 0u }; i < packageHeader.assetCount && foundOBJ == false; ++i)
		{
			PackageTool::ChunkHeader chunkHeader{};
			package.read((char*)&chunkHeader, sizeof(PackageTool::ChunkHeader));
			std::unique_ptr<char> pAssetFileName = std::unique_ptr<char>(DBG_NEW char[chunkHeader.readableSize + 1](0));
			package.read(pAssetFileName.get(), chunkHeader.readableSize);
			if (memcmp(chunkHeader.type, "MESH", 4) == 0)
			{
				PackageTool::MeshHeader meshHeader{};
				package.read((char*)&meshHeader, sizeof(PackageTool::MeshHeader));
				if (meshHeader.meshName == fileName)
				{
					std::vector<objl::Vertex> vertices;
					vertices.resize(meshHeader.verticesDataSize / sizeof(objl::Vertex));
					package.read((char*)vertices.data(), meshHeader.verticesDataSize);
					std::vector<unsigned int> indices;
					indices.resize(meshHeader.indicesDataSize / sizeof(unsigned int));
					package.read((char*)indices.data(), meshHeader.indicesDataSize);
					m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<MeshOBJ>(vertices, indices));
					return dynamic_pointer_cast<MeshOBJ>(m_Map[fileName]);
				}
				else
				{
					package.seekg(chunkHeader.chunkSize - sizeof(PackageTool::MeshHeader), std::ios_base::cur);
				}
			}
			else
			{
				package.seekg(chunkHeader.chunkSize, std::ios_base::cur);
			}
		}
		package.close();
	}
	else
	{
		//...
	}
	return nullptr;
}

const bool ResourceManager::LoadResourceFromPackage(const std::string& fileName) noexcept
{
	std::ifstream package("Packages/" + m_PackageFileMap[fileName], std::ios::binary);
	if (package.is_open())
	{
		PackageTool::PackageHeader packageHeader{};
		package.read((char*)&packageHeader, sizeof(PackageTool::PackageHeader));
		bool foundAsset = false;
		PackageTool::ChunkHeader chunkHeader{};
		for (uint32_t i{ 0u }; i < packageHeader.assetCount && foundAsset == false; ++i)
		{
			package.read((char*)&chunkHeader, sizeof(PackageTool::ChunkHeader));
			std::unique_ptr<char> pAssetFileName = std::unique_ptr<char>(DBG_NEW char[chunkHeader.readableSize + 1](0));
			package.read(pAssetFileName.get(), chunkHeader.readableSize);
			if (strcmp(pAssetFileName.get(), fileName.c_str()) == 0)
			{
				foundAsset = true;
			}
			else
			{
				package.seekg(chunkHeader.chunkSize, std::ios_base::cur);
			}
		}
		if (memcmp(chunkHeader.type, "TEX", 3) == 0) //Asset is a texture and should be loaded as such.
		{
			PackageTool::TextureHeader textureHeader{};
			package.read((char*)&textureHeader, sizeof(PackageTool::TextureHeader));
			std::unique_ptr<char> textureBuffer = std::unique_ptr<char>(DBG_NEW char[textureHeader.dataSize]);
			package.read(textureBuffer.get(), textureHeader.dataSize);
			if (memcmp(textureHeader.textureType, "NORM", 4) == 0) //Normal uncompressed texture type:
			{
				m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<Texture2D>(textureHeader.width,
																 textureHeader.height,
																 textureHeader.rowPitch,
																 textureBuffer.get(),
																 DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM));
			}
			else //Compressed texture type:
			{
				m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<Texture2D>(textureHeader.width,
																 textureHeader.height,
																 textureHeader.rowPitch,
																 textureBuffer.get(),
																 DXGI_FORMAT::DXGI_FORMAT_BC7_UNORM_SRGB));
			}
		}
		else if (memcmp(chunkHeader.type, "MESH", 4) == 0) //Asset is a mesh and should be loaded as such.
		{
			PackageTool::MeshHeader meshHeader{};
			package.read((char*)&meshHeader, sizeof(PackageTool::MeshHeader));
			std::vector<objl::Vertex> vertices;
			vertices.resize(meshHeader.verticesDataSize / sizeof(objl::Vertex));
			package.read((char*)vertices.data(), meshHeader.verticesDataSize);
			std::vector<unsigned int> indices;
			indices.resize(meshHeader.indicesDataSize / sizeof(unsigned int));
			package.read((char*)indices.data(), meshHeader.indicesDataSize);
			m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<MeshOBJ>(vertices, indices));
		}
		package.close();
		return true;
	}
	return false;
}

void ResourceManager::MapPackageContent() noexcept
{
	std::vector<std::filesystem::directory_entry> packages;
	for (const auto& package : std::filesystem::directory_iterator("Packages/"))
	{
		if (!m_PackageFileMap.contains(package.path().filename().string())) // Do not re-map a package that has already been mapped.
		{
			std::ifstream packageFile("Packages/" + package.path().filename().string(), std::ios::binary);
			if (packageFile.is_open())
			{
				PackageTool::PackageHeader pkgHdr{};
				packageFile.read((char*)&pkgHdr, sizeof(PackageTool::PackageHeader));
				for (uint32_t i{ 0u }; i < pkgHdr.assetCount; ++i)
				{
					PackageTool::ChunkHeader chkHdr{};
					packageFile.read((char*)&chkHdr, sizeof(PackageTool::ChunkHeader));
					std::unique_ptr<char> fileName = std::unique_ptr<char>(DBG_NEW char[chkHdr.readableSize + 1](0));
					packageFile.read(fileName.get(), chkHdr.readableSize);

					if (memcmp(chkHdr.type, "MESH", 4) == 0)
					{
						//Find the correct mesh name:
						PackageTool::MeshHeader meshHeader{};
						packageFile.read((char*)&meshHeader, sizeof(PackageTool::MeshHeader));
						m_PackageFileMap[meshHeader.meshName] = package.path().filename().string();
						
						m_OBJToMeshesMap[fileName.get()].push_back(meshHeader.meshName);
						if (i != pkgHdr.assetCount - 1u)
							packageFile.seekg(chkHdr.chunkSize - sizeof(PackageTool::MeshHeader), std::ios_base::cur);
					}
					else
					{
						m_PackageFileMap[fileName.get()] = package.path().filename().string();
						if (i != pkgHdr.assetCount - 1u)
							packageFile.seekg(chkHdr.chunkSize, std::ios_base::cur);
					}
				}
				packageFile.close();
			}
		}
	}
}

template<>
std::vector<std::shared_ptr<MeshOBJ>> ResourceManager::LoadMultiple(const std::string& objName) noexcept
{

	//1 Retrieve the list of all mesh names:
	std::vector<std::string> meshNames = m_OBJToMeshesMap[objName];

	//Prepare the std::vector to be returned:
	std::vector<std::shared_ptr<MeshOBJ>> meshes;

	//Have a std::vector containing flags for whether a certain mesh has been loaded or not
	std::vector<bool> meshesLoadedFlags(meshNames.size(), 0);

	//Load all meshes directly from cache that has already been loaded from the package:
	for (uint32_t i{0u}; i < meshNames.size(); ++i)
	{
		if (m_Map.contains(meshNames[i]))
		{
			meshes.push_back(dynamic_pointer_cast<MeshOBJ>(m_Map[meshNames[i]]));
			meshesLoadedFlags[i] = true;
		}
	}

	//Load the meshes from package that has not been loaded into cache:
	for (uint32_t i{ 0u }; i < meshNames.size(); i++)
	{
		if (meshesLoadedFlags[i] == false)
		{
			//This mesh has not been loaded from cache (since it didn't exist) and must now be loaded from the package:
			meshes.push_back(Load<MeshOBJ>(meshNames[i]));
		}
	}

	return meshes;
}