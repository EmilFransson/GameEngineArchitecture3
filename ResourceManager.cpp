#include "pch.h"
#include "ResourceManager.h"
#include "PackageTool.h"
#include "OBJ_Loader.h"


ResourceManager* ResourceManager::s_Instance;
std::vector<std::thread> ResourceManager::m_tWorkers;
size_t ResourceManager::m_numThreads;
std::mutex ResourceManager::m_tQueueMutex;
std::condition_variable ResourceManager::m_tCondition;
bool ResourceManager::m_tTerminate;
std::deque<ResourceManager::JobHolder*> ResourceManager::m_tQueue;

void ResourceManager::Init()
{
	if (!s_Instance)
	{
		s_Instance = DBG_NEW ResourceManager();
	}
	m_tTerminate = false;
	m_numThreads = std::thread::hardware_concurrency();
	m_tWorkers.reserve(m_numThreads);
	for (size_t i = 0; i < m_numThreads; i++)
	{
		//Maybe rework?
		m_tWorkers.push_back(std::thread(
			tWaitForJob
		));
	}
}

void ResourceManager::CleanUp()
{
	tShutdown();
	delete s_Instance;
}

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
	if (m_Map.contains(fileName))
	{
		return dynamic_pointer_cast<MeshOBJ>(m_Map[fileName]);
	}
	else
	{
		if (LoadResourceFromPackage(fileName))
		{
			return dynamic_pointer_cast<MeshOBJ>(m_Map[fileName]);
		}
		std::cout << "Error: Unable to load asset " << fileName << "\n";
		assert(false); //For now, just assert false.
	}
	return nullptr; //Should never be reached.
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
			m_Map[fileName] = dynamic_pointer_cast<Resource>(std::make_shared<Texture2D>(textureHeader.width, 
																						 textureHeader.height, 
																						 textureHeader.rowPitch, 
																						 textureBuffer.get()));
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

					m_PackageFileMap[fileName.get()] = package.path().filename().string();

					if (i != pkgHdr.assetCount - 1u)
						packageFile.seekg(chkHdr.chunkSize, std::ios_base::cur);
				}
				packageFile.close();
			}
		}
	}
}

void ResourceManager::tFindResource(std::string filename, std::string extension, std::shared_ptr<Resource>* memory)
{
	if (extension == ".obj")
	{
		*memory = MeshOBJ::Create(filename);
	}
	else if (extension == ".png")
	{
		*memory = Texture2D::Create(filename);
	}
}

void ResourceManager::tWaitForJob(
	//Arguments that doesnt change go here. (Or pointers to shared memory)
)
{
	ResourceManager* myself = ResourceManager::Get();
	JobHolder* Job;
	while (!m_tTerminate)
	{
		{
			std::unique_lock<std::mutex> lock(m_tQueueMutex);

			m_tCondition.wait(lock, [myself]()
				{
					return !m_tQueue.empty();
				});
			if (m_tTerminate)
			{
				break;
			}

			Job = m_tQueue.front();
			m_tQueue.pop_front();
		}
		Job->m_job(Job->m_filename, Job->m_extension, Job->m_memory);
		delete Job;
	}
}

void ResourceManager::tShutdown()
{
	{
		std::unique_lock<std::mutex> lock(m_tQueueMutex);
		m_tTerminate = true;
	}

	m_tCondition.notify_all();

	for (std::thread& th : m_tWorkers)
	{
		th.join();
	}

	m_tWorkers.clear();

	//stopped = true;
}