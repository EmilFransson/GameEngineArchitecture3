#include "pch.h"
#include "PackageTool.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "OBJ_Loader.h"
std::string PackageTool::Package(const char* dirPath)
{
	std::filesystem::directory_entry Folder = std::filesystem::directory_entry(dirPath);
	if (!Folder.is_directory())
	{
		assert(false);
	}

	//Open a new file for writing the package to.
	//Write a pacakge header to the file.
	std::string newPath = "Packages/" + Folder.path().filename().string() + ".pkg";
	std::ofstream packageFile(newPath);
	assert(packageFile);
	char buffer[sizeof(PackageHeader)] = { 0 };

	//Write 0's to PackageHeader
	packageFile.write(buffer, sizeof(PackageHeader));
	uint8_t assetCount = 0;
	uint32_t size = 0;
	size += sizeof(PackageHeader);

	objl::Loader loader;
	for (auto const& dir_entry : std::filesystem::directory_iterator{dirPath})
	{
		if (!dir_entry.path().has_extension())
		{
			assert(false);
		}
		
		std::string filetype = dir_entry.path().extension().string();
		
		if (filetype == ".obj")
		{
			loader.LoadFile(dir_entry.path().string());
		}
		else if (filetype == ".png")
		{
			assetCount += 1;
			auto texData = PackageTexture(dir_entry.path().string());
			ChunkHeader ch = {
				.type = {'T', 'E', 'X', ' '},
				.chunkSize = sizeof(TextureHeader) + texData.width * texData.height * 4, // 4 channels for DirectX RGBA Textures
				.readableSize = dir_entry.path().string().length()
			};
			CoCreateGuid(&ch.guid);
				
			TextureHeader th = {
				.textureType = {'C', 'O', 'L', ' '},
				.dataSize = texData.width * texData.height * 4,
				.width = texData.width,
				.height = texData.height,
				.rowPitch = texData.width * 4
			};

			//Write the chunkheader
			packageFile.write((char*)(&ch), sizeof(ChunkHeader));
			size += sizeof(ChunkHeader);
			//Write the readable
			packageFile.write(dir_entry.path().string().data(), ch.readableSize);
			size += ch.readableSize;
			//Write the textureheader
			packageFile.write((char*)(&th), sizeof(TextureHeader));
			size += sizeof(TextureHeader);
			//Write the data to the file
			packageFile.write((char*)(texData.data), th.dataSize);
			size += th.dataSize;

			//Clean up image
			stbi_image_free(texData.data);
		}
	}

	for (objl::Material currentMat : loader.LoadedMaterials)
	{
		assetCount += 1;

		ChunkHeader ch = {
				.type = {'M', 'A', 'T', ' '},
				.chunkSize = sizeof(MaterialHeader) + sizeof(objl::Material), // 4 channels for DirectX RGBA Textures
				.readableSize = sizeof(currentMat.name)
		};
		CoCreateGuid(&ch.guid);

		MaterialHeader mh = {
			.dataSize = sizeof(objl::Material)
		};

		//Write the chunkheader
		packageFile.write((char*)(&ch), sizeof(ChunkHeader));
		size += sizeof(ChunkHeader);
		//Write the readable
		packageFile.write((char*)(currentMat.name.data()), ch.readableSize); // Change to stream write operator <<
		size += ch.readableSize;
		//Write the materialheader
		packageFile.write((char*)(&mh), sizeof(MaterialHeader));
		size += sizeof(MaterialHeader);
		//Write the data to the file
		packageFile.write((char*)(&currentMat), mh.dataSize);
		size += mh.dataSize;
	}

	for (objl::Mesh currentMesh : loader.LoadedMeshes)
	{
		assetCount += 1;
		
		ChunkHeader ch = {
				.type = {'M', 'E', 'S', 'H'},
				.chunkSize = sizeof(MeshHeader) + sizeof(currentMesh.Vertices) + sizeof(currentMesh.Indices), // 4 channels for DirectX RGBA Textures
				.readableSize = sizeof(currentMesh.MeshName)
		};
		CoCreateGuid(&ch.guid);

		MeshHeader mh = {
				.materialName = currentMesh.MeshMaterial.name,
				.verticesDataSize = sizeof(currentMesh.Vertices),
				.indicesDataSize = sizeof(currentMesh.Indices)
		};

		//Write the chunkheader
		packageFile.write((char*)(&ch), sizeof(ChunkHeader));
		size += sizeof(ChunkHeader);
		//Write the readable
		packageFile.write((char*)(currentMesh.MeshName.data()), ch.readableSize);
		size += ch.readableSize;
		//Write the textureheader
		packageFile.write((char*)(&mh), sizeof(MeshHeader));
		size += sizeof(MeshHeader);
		//Write the vertices data to the file
		packageFile.write((char*)(&currentMesh.Vertices), sizeof(currentMesh.Vertices));
		size += sizeof(currentMesh.Vertices);
		//Write the indices data to the file
		packageFile.write((char*)(&currentMesh.Indices), sizeof(currentMesh.Indices));
		size += sizeof(currentMesh.Indices);
	}

	//Update Package Header
	packageFile.seekp(0, std::ios_base::beg);
	PackageHeader ph = {
		.id = { 'P', 'K', 'G'},
		.assetCount = assetCount,
		.size = size
	};
	packageFile.write((char*)(&ph), sizeof(PackageHeader));

	packageFile.close();
	assert(!packageFile.fail());

	//Zip the package file.

	//Return the filepath to the zip.
	return newPath;
}

PackageTool::PackagedTexture PackageTool::PackageTexture(const std::string& texPath)
{
	PackagedTexture tex = {0};
	int width, height, channels;
	auto imageData = stbi_load(texPath.c_str(), &width, &height, &channels, 0);
	
	// .. Compress Through DirectXTex or something ..

	// TODO: stbi_image_free(imageData); // Uncomment this when the above compression has been accomplished
	// REMOVE ONCE COMPRESSION HAS BEEN ACCOMPLISHED
	tex.width = width;
	tex.height = height;
	tex.rowPitch = width * channels;
	tex.data = imageData;
	return tex;
}
