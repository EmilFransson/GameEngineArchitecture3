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
	std::shared_ptr<ResourceType> Load(const std::string& fileName) noexcept;
	template<>
	std::shared_ptr<Texture2D> Load(const std::string& fileName) noexcept;
	template<>
	std::shared_ptr<MeshOBJ> Load(const std::string& fileName) noexcept;
	template<>
	std::shared_ptr<Material> Load(const std::string& materialName) noexcept;
	[[nodiscard]] const bool LoadResourceFromPackage(const std::string& fileName) noexcept;
	void MapPackageContent() noexcept;
	template<typename ResourceType>
	std::vector<std::shared_ptr<ResourceType>> LoadMultiple(const std::string& fileName) noexcept {}
	template<>
	std::vector<std::shared_ptr<MeshOBJ>> LoadMultiple(const std::string& objName) noexcept;
private:
	ResourceManager() noexcept = default;
	~ResourceManager() noexcept = default;
private:
	static ResourceManager s_Instance;
	std::map<std::string, std::shared_ptr<Resource>> m_Map;
	std::map<std::string, std::string> m_PackageFileMap;
	std::map<std::string, std::vector<std::string>> m_OBJToMeshesMap;
};

template<typename ResourceType>
std::shared_ptr<ResourceType> ResourceManager::Load(const std::string& filePath) noexcept
{
	// What should happen if loading a resource not supported...?
}
