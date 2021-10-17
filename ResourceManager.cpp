#include "pch.h"
#include "ResourceManager.h"
#include "PackageTool.h"
#include "OBJ_Loader.h"

/* Include low level I/O */
#include <io.h>
#include <fcntl.h>

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
		if (m_PackageFileMap.contains(fileName))
		{
			if (LoadResourceFromPackage(fileName))
			{
				return dynamic_pointer_cast<Texture2D>(m_Map[fileName]);
			}
		}
		else
		{
			std::cout << "Error: Unable to load asset " << fileName << ". Placeholder loaded instead.\n";
			if (m_Map.contains("Grey.png"))
			{
				return dynamic_pointer_cast<Texture2D>(m_Map["Grey.png"]);
			}
			else
			{
				if (LoadResourceFromPackage("Grey.png"))
				{
					return dynamic_pointer_cast<Texture2D>(m_Map["Grey.png"]);
				}
			}
		}
	}
	return nullptr; //Should never be reached.
}

template<>
std::shared_ptr<MeshOBJ> ResourceManager::Load(const std::string& fileName) noexcept
{
	int package_fd;
	std::string filePath = "Packages/" + m_PackageFileMap[fileName];
	errno_t err = _sopen_s(&package_fd, filePath.c_str(), _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
	if (err == 0)
	{
		PackageTool::PackageHeader packageHeader{};
		int bytes_read = _read(package_fd, (char*)&packageHeader, sizeof(PackageTool::PackageHeader));
		bool foundOBJ = false;
		for (uint32_t i{0u}; i < packageHeader.assetCount && foundOBJ == false; ++i)
		{
			PackageTool::ChunkHeader chunkHeader{};
			bytes_read = _read(package_fd, (char*)&chunkHeader, sizeof(PackageTool::ChunkHeader));
			_lseek(package_fd, static_cast<long>(chunkHeader.readableSize), SEEK_CUR); // Skip readable name
			if (memcmp(chunkHeader.type, "MESH", 4) == 0)
			{
				PackageTool::MeshHeader meshHeader{};
				bytes_read = _read(package_fd, (char*)&meshHeader, sizeof(PackageTool::MeshHeader));
				if (meshHeader.meshName == fileName)
				{
					std::vector<objl::Vertex> vertices;
					vertices.resize(meshHeader.verticesDataSize / sizeof(objl::Vertex));
					bytes_read = _read(package_fd, (char*)vertices.data(), static_cast<unsigned int>(meshHeader.verticesDataSize));
					std::vector<unsigned int> indices;
					indices.resize(meshHeader.indicesDataSize / sizeof(unsigned int));
					bytes_read = _read(package_fd, (char*)indices.data(), static_cast<unsigned int>(meshHeader.indicesDataSize));
					if (strcmp(meshHeader.materialName, "") != 0)
					{
						m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<MeshOBJ>(vertices, 
																								   indices, 
																								   Load<Material>(meshHeader.materialName)));
					}
					else
					{
						m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<MeshOBJ>(vertices, indices));
					}
					return dynamic_pointer_cast<MeshOBJ>(m_Map[fileName]);
				}
				else
				{
					_lseek(package_fd, chunkHeader.chunkSize - sizeof(PackageTool::MeshHeader), SEEK_CUR);
				}
			}
			else
			{
				_lseek(package_fd, chunkHeader.chunkSize, SEEK_CUR);
			}
		}
		_close(package_fd);
	}
	else
	{
		//...
	}
	return nullptr;
}

template<>
std::shared_ptr<Material> ResourceManager::Load(const std::string& materialName) noexcept
{
	if (m_Map.contains(materialName))
	{
		return dynamic_pointer_cast<Material>(m_Map[materialName]);
	}
	else
	{
		int package_fd;
		std::string filePath = "Packages/" + m_PackageFileMap[materialName];
		errno_t err = _sopen_s(&package_fd, filePath.c_str(), _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
		if (err == 0)
		{
			PackageTool::PackageHeader packageHeader{};
			int bytes_read = _read(package_fd, (char*)&packageHeader, sizeof(PackageTool::PackageHeader));
			bool foundMaterial = false;
			PackageTool::MaterialHeader materialHeader{};
			for (uint32_t i{ 0u }; i < packageHeader.assetCount && foundMaterial == false; ++i)
			{
				PackageTool::ChunkHeader chunkHeader{};
				bytes_read = _read(package_fd, (char*)&chunkHeader, sizeof(PackageTool::ChunkHeader));
				_lseek(package_fd, static_cast<long>(chunkHeader.readableSize), SEEK_CUR); // Skip readable name
				if (memcmp(chunkHeader.type, "MAT", 3) == 0)
				{
					bytes_read = _read(package_fd, (char*)&materialHeader, sizeof(PackageTool::MaterialHeader));
					if (strcmp(materialHeader.materialName, materialName.c_str()) == 0)
					{
						foundMaterial = true;
					}
					else
					{
						_lseek(package_fd, chunkHeader.chunkSize - sizeof(PackageTool::MaterialHeader), SEEK_CUR);
					}
				}
				else
				{
					_lseek(package_fd, chunkHeader.chunkSize, SEEK_CUR);
				}
			}
			if (foundMaterial)
			{
				PackageTool::SMaterial material{};
				bytes_read = _read(package_fd, (char*)&material, materialHeader.dataSize);
				m_Map[materialName] = dynamic_pointer_cast<Resource>(std::make_shared<Material>(material));
				return dynamic_pointer_cast<Material>(m_Map[materialName]);
			}
		}
	}
	return nullptr; //Should never be reached.
}

const bool ResourceManager::LoadResourceFromPackage(const std::string& fileName) noexcept
{
	int package_fd;
	std::string filePath = "Packages/";
	filePath += std::string(m_PackageFileMap[fileName]);
	errno_t err = _sopen_s(&package_fd, filePath.c_str(), _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
	if(err == 0)
	{
		PackageTool::PackageHeader packageHeader{};
		int bytes_read = _read(package_fd, (char*)&packageHeader, sizeof(PackageTool::PackageHeader));
		bool foundAsset = false;
		PackageTool::ChunkHeader chunkHeader{};
		for (uint32_t i{ 0u }; i < packageHeader.assetCount && foundAsset == false; ++i)
		{
			bytes_read = _read(package_fd, (char*)&chunkHeader, sizeof(PackageTool::ChunkHeader));
			std::unique_ptr<char> pAssetFileName = std::unique_ptr<char>(DBG_NEW char[chunkHeader.readableSize + 1](0));
			bytes_read = _read(package_fd, pAssetFileName.get(), static_cast<unsigned int>(chunkHeader.readableSize));
			if (std::string(pAssetFileName.get()) == fileName)
			{
				foundAsset = true;
			}
			else
			{
				_lseek(package_fd, chunkHeader.chunkSize, SEEK_CUR);
			}
		}
		if (memcmp(chunkHeader.type, "TEX", 3) == 0) //Asset is a texture and should be loaded as such.
		{
			PackageTool::TextureHeader textureHeader{};
			bytes_read = _read(package_fd, (char*)&textureHeader, sizeof(PackageTool::TextureHeader));
			std::unique_ptr<char> textureBuffer = std::unique_ptr<char>(DBG_NEW char[textureHeader.dataSize]);
			bytes_read = _read(package_fd, textureBuffer.get(), textureHeader.dataSize);
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
		_close(package_fd);
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
			int package_fd;
			std::string packagePath = "Packages/" + package.path().filename().string();
			errno_t err = _sopen_s(&package_fd, packagePath.c_str(), _O_BINARY | _O_RDONLY, _SH_DENYWR, _S_IREAD);
			if (err == 0)
			{
				PackageTool::PackageHeader pkgHdr{};
				int bytes_read = _read(package_fd, (char*)&pkgHdr, sizeof(PackageTool::PackageHeader));
				for (uint32_t i{ 0u }; i < pkgHdr.assetCount; ++i)
				{
					PackageTool::ChunkHeader chkHdr{};
					bytes_read = _read(package_fd, (char*)&chkHdr, sizeof(PackageTool::ChunkHeader));
					std::unique_ptr<char> fileName = std::unique_ptr<char>(DBG_NEW char[chkHdr.readableSize + 1](0));
					bytes_read = _read(package_fd, fileName.get(), static_cast<unsigned int>(chkHdr.readableSize));

					if (memcmp(chkHdr.type, "MESH", 4) == 0)
					{
						//Find the correct mesh name:
						PackageTool::MeshHeader meshHeader{};
						bytes_read = _read(package_fd, (char*)&meshHeader, sizeof(PackageTool::MeshHeader)); 
						m_PackageFileMap[meshHeader.meshName] = package.path().filename().string();
						
						m_OBJToMeshesMap[fileName.get()].push_back(meshHeader.meshName);
						if (i != pkgHdr.assetCount - 1u)
							_lseek(package_fd, chkHdr.chunkSize - sizeof(PackageTool::MeshHeader), SEEK_CUR);
					}
					else if (memcmp(chkHdr.type, "MAT", 3) == 0)
					{
						PackageTool::MaterialHeader materialHeader{};
						bytes_read = _read(package_fd, (char*)&materialHeader, sizeof(PackageTool::MaterialHeader));
						m_PackageFileMap[materialHeader.materialName] = package.path().filename().string();
						if (i != pkgHdr.assetCount - 1u)
							_lseek(package_fd, chkHdr.chunkSize - sizeof(PackageTool::MaterialHeader), SEEK_CUR);
					}
					else
					{
						m_PackageFileMap[fileName.get()] = package.path().filename().string();
						if (i != pkgHdr.assetCount - 1u)
							_lseek(package_fd, chkHdr.chunkSize, SEEK_CUR);
					}
				}
				_close(package_fd);
			}
		}
	}
}

template<>
std::vector<std::shared_ptr<MeshOBJ>> ResourceManager::LoadMultiple(const std::string& objName) noexcept
{
	std::vector<std::string> meshNames;
	//1 Retrieve the list of all mesh names:
	if (m_OBJToMeshesMap.contains(objName))
	{
		meshNames = m_OBJToMeshesMap[objName];
	}
	else
	{
		//Instead add placeholder.
		std::cout << "Error: Unable to load asset " << objName << ". Placeholder loaded instead.\n";
		meshNames = m_OBJToMeshesMap["Cube.obj"];
	}

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
			meshes.push_back(std::move(Load<MeshOBJ>(meshNames[i])));
		}
	}
	return meshes;
}