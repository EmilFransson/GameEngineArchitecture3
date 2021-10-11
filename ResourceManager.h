#pragma once
#include "Texture.h"
#include "Mesh.h"

struct PackageHeader
{
	char id[3]; //PCK or PKG ow whatever, not strictly necessary but feels official.
	uint8_t assetCount;
	uint32_t size;
};

struct ChunkHeader
{
	char type[4];		//Ex: TEX, MESH, AUD...
	uint32_t chunkSize;	//Chunk header not included.
	GUID guid;
	uint32_t lengthOfHumanReadableString;
};

struct TextureHeader
{
	char textureType[4]; //COL, NORM, SPEC ...
	uint32_t dataSize;
	uint16_t width, height;
	uint16_t rowPitch;

	uint16_t pad;
};

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
	std::shared_ptr<ResourceType> Load(const std::string_view filePath) noexcept;
	template<>
	std::shared_ptr<Texture2D> Load(const std::string_view filePath) noexcept
	{
		if (m_Map.contains(filePath))
		{
			return dynamic_pointer_cast<Texture2D>(m_Map[filePath]);
		}
		else
		{
			std::ifstream pkg("test.pkg", std::ios::binary);
			if (pkg.is_open())
			{
				PackageHeader pkghdr{};
				pkg.read((char*)&pkghdr, sizeof(PackageHeader));
				ChunkHeader chHdr{};
				pkg.read((char*)&chHdr, sizeof(ChunkHeader));
				std::unique_ptr<char> fileName = std::unique_ptr<char>(DBG_NEW char[chHdr.lengthOfHumanReadableString + 1]);
				pkg.read(fileName.get(), chHdr.lengthOfHumanReadableString);
				fileName.get()[chHdr.lengthOfHumanReadableString] = '\0';
				if (strcmp(fileName.get(), filePath.data()) != 0)
				{
					__debugbreak(); // temporary
				}
				TextureHeader texHdr{};
				pkg.read((char*)&texHdr, sizeof(TextureHeader));
				std::unique_ptr<char> textureBuffer = std::unique_ptr<char>(DBG_NEW char[texHdr.dataSize]);
				pkg.read(textureBuffer.get(), texHdr.dataSize);
				pkg.close();

				std::shared_ptr<Texture2D> resourceType = std::make_shared<Texture2D>(texHdr.width, texHdr.height, texHdr.rowPitch, textureBuffer.get());
				
				std::shared_ptr<Resource> resource = dynamic_pointer_cast<Resource>(resourceType);
				m_Map[filePath] = resource;

				return resourceType;
			}
			return nullptr;
		}
	}
	template<>
	std::shared_ptr<MeshOBJ> Load(const std::string_view filePath) noexcept
	{
		if (m_Map.contains(filePath))
		{
			return dynamic_pointer_cast<MeshOBJ>(m_Map[filePath]);
		}
		else
		{
			std::shared_ptr<MeshOBJ> resourceType = std::make_shared<MeshOBJ>();
			std::shared_ptr<Resource> resource = dynamic_pointer_cast<Resource>(resourceType);

			m_Map[filePath] = resource;
			return resourceType;
		}
	}
private:
	ResourceManager() noexcept = default;
	~ResourceManager() noexcept = default;
private:
	static ResourceManager s_Instance;
	std::map<std::string_view, std::shared_ptr<Resource>> m_Map;
};

 template<typename ResourceType>
 std::shared_ptr<ResourceType> ResourceManager::Load(const std::string_view filePath) noexcept
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
