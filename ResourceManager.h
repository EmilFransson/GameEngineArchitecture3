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
	template<>
	std::shared_ptr<MeshOBJ> Load(const std::string& filePath) noexcept;
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
