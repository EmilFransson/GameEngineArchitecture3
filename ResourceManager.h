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
	static ResourceManager* Get() { return s_Instance; }
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

	//Used for thread initialization and cleanup.
	void Init();
	void CleanUp();
private:
	ResourceManager() noexcept
	{
		s_Instance = this;
	}
	~ResourceManager() noexcept = default;
private:
	static ResourceManager* s_Instance;

	std::map<std::pair<uint64_t, uint64_t>, std::shared_ptr<Resource>> m_GUIDToResourceMap;
	std::map<std::pair<uint64_t, uint64_t>, std::string> m_GUIDToPackageMap;
	std::map<std::string, std::vector<std::string>> m_OBJToMeshesMap;
private:
	struct JobHolder
	{
		JobHolder() = default;
		JobHolder(std::function<void(std::string, std::string, std::shared_ptr<Resource>*)> newJob, std::string filename, std::string extension, std::shared_ptr<Resource>* memory)
		{
			m_job = newJob;
			m_filename = filename;
			m_extension = extension;
			m_memory = memory;
		}
		std::function<void(std::string, std::string, std::shared_ptr<Resource>*)> m_job;
		std::string m_filename;
		std::string m_extension;
		std::shared_ptr<Resource>* m_memory;
	};
	static std::vector<std::thread> m_tWorkers;
	static size_t m_numThreads;

	static std::deque<JobHolder*> m_tQueue;

	static std::mutex m_tQueueMutex;
	static std::condition_variable m_tCondition;
	static bool m_tTerminate;

	static void tWaitForJob();		//The threadfunction.
	static void tFindResource(std::string, std::string, std::shared_ptr<Resource>*);	//Function that is sent into the addJob function. The actual "work" for the thread.
	void tShutdown();		//Called at shutdown.
public:
	template <typename T>
	void tAddJob(std::string, std::shared_ptr<T>*);	//Called to add a new job to the queue for the threads to do.
};

template <typename T>
void ResourceManager::tAddJob(std::string filename, std::shared_ptr<T>* memory)
{
	std::string extension = filename.substr(filename.find_last_of('.'), filename.size() - 1);
	if (extension == ".obj")
	{
		//Set placeholder model in memory parameter.

	}
	else if (extension == ".png")
	{
		//Set placeholder texture in memory parameter.
		*memory = Texture2D::Create("thanos.png");
	}

	//Add the job to the queue.
	{
		std::unique_lock<std::mutex> lock(m_tQueueMutex);
		m_tQueue.push_back(DBG_NEW JobHolder(tFindResource, filename, extension, reinterpret_cast<std::shared_ptr<Resource>*>(memory)));
	}
	//Notify a waiting thread.
	m_tCondition.notify_one();
}


template<typename ResourceType>
std::shared_ptr<ResourceType> ResourceManager::Load(const std::pair<uint64_t, uint64_t>& guid) noexcept
{
	// Base for template specialization
}
