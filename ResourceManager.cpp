#include "pch.h"
#include "ResourceManager.h"
#include "PackageTool.h"
#include "OBJ_Loader.h"

/* Include low level I/O */
#include <io.h>
#include <fcntl.h>

ResourceManager ResourceManager::s_Instance;

template<>
std::shared_ptr<Texture2D> ResourceManager::Load(const std::pair<uint64_t, uint64_t>& guid) noexcept
{
	if (m_GUIDToResourceMap.contains(guid))
	{
		return dynamic_pointer_cast<Texture2D>(m_GUIDToResourceMap[guid]);
	}
	else
	{
		if (m_GUIDToPackageMap.contains(guid))
		{
			if (LoadResourceFromPackage(guid))
			{
				return dynamic_pointer_cast<Texture2D>(m_GUIDToResourceMap[guid]);
			}
		}
		else
		{
			std::cout << "Error: Unable to load asset " <<  "Placeholder loaded instead.\n";
			if (m_GUIDToResourceMap.contains(ConvertGUIDToPair(m_FileNameToGUIDMap["Grey.png"])))
			{
				return dynamic_pointer_cast<Texture2D>(m_GUIDToResourceMap[ConvertGUIDToPair(m_FileNameToGUIDMap["Grey.png"])]);
			}
			else
			{
				if (LoadResourceFromPackage(ConvertGUIDToPair(m_FileNameToGUIDMap["Grey.png"])))
				{
					return dynamic_pointer_cast<Texture2D>(m_GUIDToResourceMap[ConvertGUIDToPair(m_FileNameToGUIDMap["Grey.png"])]);
				}
			}
		}
	}
	return nullptr; //Should never be reached.
}

template<>
std::shared_ptr<MeshOBJ> ResourceManager::Load(const std::pair<uint64_t, uint64_t>& guid) noexcept
{
	int package_fd;
	std::string filePath = "Packages/" + std::string(m_GUIDToPackageMap[guid]);
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
				if (ConvertGUIDToPair(chunkHeader.guid) == guid)
				{
					std::vector<objl::Vertex> vertices;
					vertices.resize(meshHeader.verticesDataSize / sizeof(objl::Vertex));
					bytes_read = _read(package_fd, (char*)vertices.data(), static_cast<unsigned int>(meshHeader.verticesDataSize));
					std::vector<unsigned int> indices;
					indices.resize(meshHeader.indicesDataSize / sizeof(unsigned int));
					bytes_read = _read(package_fd, (char*)indices.data(), static_cast<unsigned int>(meshHeader.indicesDataSize));
					if (strcmp(meshHeader.materialName, "") != 0)
					{
						m_GUIDToResourceMap[guid] = dynamic_pointer_cast<Resource>(std::make_shared<MeshOBJ>(vertices,
																				   indices, 
																				   Load<Material>(ConvertGUIDToPair(m_FileNameToGUIDMap[meshHeader.materialName]))));
					}
					else
					{
						m_GUIDToResourceMap[guid] = dynamic_pointer_cast<Resource>(std::make_shared<MeshOBJ>(vertices, indices));
					}
					return dynamic_pointer_cast<MeshOBJ>(m_GUIDToResourceMap[guid]);
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
std::shared_ptr<Material> ResourceManager::Load(const std::pair<uint64_t, uint64_t>& guid) noexcept
{
	if (m_GUIDToResourceMap.contains(guid))
	{
		return dynamic_pointer_cast<Material>(m_GUIDToResourceMap[guid]);
	}
	else
	{
		int package_fd;
		std::string filePath = "Packages/" + std::string(m_GUIDToPackageMap[guid]);
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
					if (ConvertGUIDToPair(chunkHeader.guid) == guid)
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
				m_GUIDToResourceMap[guid] = dynamic_pointer_cast<Resource>(std::make_shared<Material>(material));
				return dynamic_pointer_cast<Material>(m_GUIDToResourceMap[guid]);
			}
		}
	}
	return nullptr; //Should never be reached.
}

const bool ResourceManager::LoadResourceFromPackage(const std::pair<uint64_t, uint64_t>& guid) noexcept
{
	int package_fd;
	std::string filePath = "Packages/";
	filePath += std::string(m_GUIDToPackageMap[guid]);
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
			if (ConvertGUIDToPair(chunkHeader.guid) == guid)
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
				m_GUIDToResourceMap[guid] = dynamic_pointer_cast<Resource>(std::make_shared<Texture2D>(textureHeader.width,
																 textureHeader.height,
																 textureHeader.rowPitch,
																 textureBuffer.get(),
																 DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM));
			}
			else //Compressed texture type:
			{
				m_GUIDToResourceMap[guid] = dynamic_pointer_cast<Resource>(std::make_shared<Texture2D>(textureHeader.width,
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

					m_GUIDToPackageMap[ConvertGUIDToPair(chkHdr.guid)] = package.path().filename().string();
					m_FileNameToGUIDMap[meshHeader.meshName] = chkHdr.guid;
					m_OBJToMeshesMap[fileName.get()].push_back(meshHeader.meshName);
					if (i != pkgHdr.assetCount - 1u)
						_lseek(package_fd, chkHdr.chunkSize - sizeof(PackageTool::MeshHeader), SEEK_CUR);
				}
				else if (memcmp(chkHdr.type, "MAT", 3) == 0)
				{
					PackageTool::MaterialHeader materialHeader{};
					bytes_read = _read(package_fd, (char*)&materialHeader, sizeof(PackageTool::MaterialHeader));
					m_GUIDToPackageMap[ConvertGUIDToPair(chkHdr.guid)] = package.path().filename().string();
					m_FileNameToGUIDMap[materialHeader.materialName] = chkHdr.guid;
					if (i != pkgHdr.assetCount - 1u)
						_lseek(package_fd, chkHdr.chunkSize - sizeof(PackageTool::MaterialHeader), SEEK_CUR);
				}
				else
				{
					m_GUIDToPackageMap[ConvertGUIDToPair(chkHdr.guid)] = package.path().filename().string();
					m_FileNameToGUIDMap[fileName.get()] = chkHdr.guid;
					if (i != pkgHdr.assetCount - 1u)
						_lseek(package_fd, chkHdr.chunkSize, SEEK_CUR);
				}
			}
			_close(package_fd);
		}
	}
}

std::pair<uint64_t, uint64_t> ResourceManager::ConvertGUIDToPair(const GUID& guid) noexcept
{
	auto split = (uint64_t*)&guid;
	return std::make_pair(split[0], split[1]);
}

GUID ResourceManager::ConvertPairToGUID(const std::pair<uint64_t, uint64_t> pGuid) noexcept
{
	auto guid = *(GUID*)&pGuid;
	
	return guid;
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
		meshNames = m_OBJToMeshesMap["Cube.obj"];
	}

	//Prepare the std::vector to be returned:
	std::vector<std::shared_ptr<MeshOBJ>> meshes;

	//Have a std::vector containing flags for whether a certain mesh has been loaded or not
	std::vector<bool> meshesLoadedFlags(meshNames.size(), 0);

	//Load all meshes directly from cache that has already been loaded from the package:
	for (uint32_t i{0u}; i < meshNames.size(); ++i)
	{
		if (m_GUIDToResourceMap.contains(ConvertGUIDToPair(m_FileNameToGUIDMap[meshNames[i]])))
		{
			meshes.push_back(dynamic_pointer_cast<MeshOBJ>(m_GUIDToResourceMap[ConvertGUIDToPair(m_FileNameToGUIDMap[meshNames[i]])]));
			meshesLoadedFlags[i] = true;
		}
	}

	//Load the meshes from package that has not been loaded into cache:
	for (uint32_t i{ 0u }; i < meshNames.size(); i++)
	{
		if (meshesLoadedFlags[i] == false)
		{
			//This mesh has not been loaded from cache (since it didn't exist) and must now be loaded from the package:
			meshes.push_back(std::move(Load<MeshOBJ>(ConvertGUIDToPair(m_FileNameToGUIDMap[meshNames[i]]))));
		}
	}
	return meshes;
}