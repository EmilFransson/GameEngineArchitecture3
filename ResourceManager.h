#pragma once
//#include "PackageTool.h"
#include "Texture.h"
#include "Mesh.h"

class ResourceManager
{
public:
#pragma region Deleted ctors
	ResourceManager(const ResourceManager& other) = delete;
	const ResourceManager& operator=(const ResourceManager& other) = delete;
	ResourceManager(const ResourceManager&& other) = delete;
	const ResourceManager& operator=(const ResourceManager&& other) = delete;
#pragma endregion
	static ResourceManager& Get() { return s_Instance; }
	template<typename ResourceType>
	std::shared_ptr<ResourceType> Load(const std::string& filePath) noexcept;
	template<>
	std::shared_ptr<Texture2D> Load(const std::string& filePath) noexcept;
	//{
	//	if (m_Map.contains(filePath))
	//	{
	//		return dynamic_pointer_cast<Texture2D>(m_Map[filePath]);
	//	}
	//	else
	//	{
	//		std::ifstream pkg("Packages/" + m_PackageFileMap[filePath], std::ios::binary);
	//		if (pkg.is_open())
	//		{
	//			PackageTool::PackageHeader pkghdr{};
	//			pkg.read((char*)&pkghdr, sizeof(PackageTool::PackageHeader));
	//			bool foundTexture = false;
	//			for (uint32_t i{ 0u }; i < pkghdr.assetCount && foundTexture == false; ++i)
	//			{
	//				PackageTool::ChunkHeader chHdr{};
	//				pkg.read((char*)&chHdr, sizeof(PackageTool::ChunkHeader));
	//				std::unique_ptr<char> fileName = std::unique_ptr<char>(DBG_NEW char[chHdr.readableSize + 1]);
	//				pkg.read(fileName.get(), chHdr.readableSize);
	//				fileName.get()[chHdr.readableSize] = '\0';
	//				if (strcmp(fileName.get(), filePath.data()) != 0)
	//				{
	//					pkg.seekg(chHdr.chunkSize, std::ios_base::cur);
	//				}
	//				else
	//				{
	//					foundTexture = true;
	//				}
	//			}
	//			if (foundTexture == true)
	//			{
	//				PackageTool::TextureHeader texHdr{};
	//				pkg.read((char*)&texHdr, sizeof(PackageTool::TextureHeader));
	//				std::unique_ptr<char> textureBuffer = std::unique_ptr<char>(DBG_NEW char[texHdr.dataSize]);
	//				pkg.read(textureBuffer.get(), texHdr.dataSize);
	//				pkg.close();
	//
	//				std::shared_ptr<Texture2D> resourceType = std::make_shared<Texture2D>(texHdr.width, texHdr.height, texHdr.rowPitch, textureBuffer.get());
	//
	//				std::shared_ptr<Resource> resource = dynamic_pointer_cast<Resource>(resourceType);
	//				m_Map[filePath] = resource;
	//
	//				return resourceType;
	//			}
	//			else
	//			{
	//				return nullptr;
	//			}
	//		}
	//		return nullptr;
	//	}
	//}
	template<>
	std::shared_ptr<MeshOBJ> Load(const std::string& filePath) noexcept;
	//{
	//	if (m_Map.contains(filePath))
	//	{
	//		return dynamic_pointer_cast<MeshOBJ>(m_Map[filePath]);
	//	}
	//	else
	//	{
	//		std::ifstream pkg("Packages/" + m_PackageFileMap[filePath], std::ios::binary);
	//		if (pkg.is_open())
	//		{
	//			PackageTool::PackageHeader pkghdr{};
	//			pkg.read((char*)&pkghdr, sizeof(PackageTool::PackageHeader));
	//			bool foundOBJ = false;
	//			for (uint32_t i{ 0u }; i < pkghdr.assetCount && foundOBJ == false; ++i)
	//			{
	//				PackageTool::ChunkHeader chkHdr{};
	//				pkg.read((char*)&chkHdr, sizeof(PackageTool::ChunkHeader));
	//				if (strcmp(chkHdr.type, "MESH") == 0)
	//				{
	//					std::unique_ptr<char> pFileName = std::unique_ptr<char>(DBG_NEW char[chkHdr.readableSize + 1]);
	//					pkg.read(pFileName.get(), chkHdr.readableSize);
	//					pFileName.get()[chkHdr.readableSize] = '\0';
	//					if (strcmp(pFileName.get(), filePath.c_str()) == 0)
	//					{
	//						foundOBJ = true;
	//					}
	//					else
	//					{
	//						if (i != pkghdr.assetCount - 1u)
	//						{
	//							pkg.seekg(chkHdr.chunkSize, std::ios_base::cur);
	//						}
	//					}
	//				}
	//			}
	//			if (foundOBJ)
	//			{
	//				PackageTool::MeshHeader meshHdr{};
	//				pkg.read((char*)&meshHdr, sizeof(PackageTool::MeshHeader));
	//				std::vector<objl::Vertex> vertices;
	//				pkg.read((char*)&vertices, meshHdr.verticesDataSize);
	//				std::vector<unsigned int> indices;
	//				pkg.read((char*)&indices, meshHdr.indicesDataSize);
	//			}
	//			
	//			pkg.close();
	//
	//
	//		}
	//		std::shared_ptr<MeshOBJ> resourceType = std::make_shared<MeshOBJ>();
	//		std::shared_ptr<Resource> resource = dynamic_pointer_cast<Resource>(resourceType);
	//
	//		m_Map[filePath] = resource;
	//		return resourceType;
	//	}
	//}
	void MapPackageContent() noexcept;
private:
	ResourceManager() noexcept = default;
	~ResourceManager() noexcept = default;
private:
	static ResourceManager s_Instance;
	std::map<std::string_view, std::shared_ptr<Resource>> m_Map;
	std::map<std::string, std::string> m_PackageFileMap;
};

 template<typename ResourceType>
 std::shared_ptr<ResourceType> ResourceManager::Load(const std::string& filePath) noexcept
 {
	 static_assert(std::is_base_of<Resource, ResourceType>::value, "Error: ResourceType is not a child of Resource.");
	 if (m_Map.contains(filePath))
	 {
		 //auto& [guid, resource] = m_Map[filePath];

		 return dynamic_pointer_cast<ResourceType>(m_Map[filePath]);
	 }
	 else
	 {
		 GUID aGUID;
		 CoCreateGuid(&aGUID);
		 std::shared_ptr<ResourceType> resourceType = std::make_shared<ResourceType>();
		 std::shared_ptr<Resource> resource = dynamic_pointer_cast<Resource>(resourceType);
		 //Perform real set up of resource
		 //resource->Load(...)

		 m_Map[filePath] = resource;

		 return resourceType;
	 }
 }
