#pragma once
#include "Texture.h"
#include "Mesh.h"
#include "Material.h"

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
	std::shared_ptr<ResourceType> Load(const std::pair<uint64_t, uint64_t>& guid) noexcept;
	template<>
	std::shared_ptr<Texture2D> Load(const std::pair<uint64_t, uint64_t>& guid) noexcept;
	template<>
	std::shared_ptr<MeshOBJ> Load(const std::pair<uint64_t, uint64_t>& guid) noexcept;
	template<>
	std::shared_ptr<Material> Load(const std::pair<uint64_t, uint64_t>& guid) noexcept;
	[[nodiscard]] const bool LoadResourceFromPackage(const std::pair<uint64_t, uint64_t>& guid) noexcept;
	void MapPackageContent() noexcept;
	template<typename ResourceType>
	std::vector<std::shared_ptr<ResourceType>> LoadMultiple(const std::string& fileName) noexcept {}
	template<>
	std::vector<std::shared_ptr<MeshOBJ>> LoadMultiple(const std::string& objName) noexcept;
	std::pair<uint64_t, uint64_t> ConvertGUIDToPair(const GUID& guid) noexcept;
	GUID ConvertPairToGUID(const std::pair<uint64_t, uint64_t> pGuid) noexcept;
	std::map<std::string, GUID> m_FileNameToGUIDMap;
private:
	ResourceManager() noexcept = default;
	~ResourceManager() noexcept = default;
private:
	static ResourceManager s_Instance;
	std::map<std::pair<uint64_t, uint64_t>, std::shared_ptr<Resource>> m_GUIDToResourceMap;
	std::map<std::pair<uint64_t, uint64_t>, std::string> m_GUIDToPackageMap;
	std::map<std::string, std::vector<std::string>> m_OBJToMeshesMap;
};

template<typename ResourceType>
std::shared_ptr<ResourceType> ResourceManager::Load(const std::pair<uint64_t, uint64_t>& guid) noexcept
{
	// Base for template specialization
}
