#include "pch.h"
#include "PackageTool.h"
#include "stb_image.h"

void PackageTool::Package(const std::string& dirPath)
{
	std::filesystem::directory_entry Folder = std::filesystem::directory_entry(dirPath);
	if (!Folder.is_directory())
	{
		assert(false);
	}

	//Open a new file for writing the package to.
	//Write a pacakge header to the file.
	std::ofstream packageFile("../Packages/" + Folder.path().filename().string());
	assert(packageFile);
	
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
			auto texData = PackageTexture(dir_entry.path().string());
			ChunkHeader ch = {
				.type = {'T', 'E', 'X', ' '},
				.chunkSize = sizeof(TextureHeader) + texData.width * texData.height * 4, // 4 channels for DirectX RGBA Textures
				.readableSize = dir_entry.path().string().length(),
			};
			CoCreateGuid(&ch.guid);
				
			//Write a texture header to the file
			//Write the data to the file
			//Clean up image
		}

		for (objl::Material currentMat : loader.LoadedMaterials)
		{
			//Write the material to the opened package file
			//Write a chunk header to the file
			//Write a Material header to the file
			//Write the data to the file
		}
		
		for (objl::Mesh currentMesh : loader.LoadedMeshes)
		{
			//F�rst s� g�r vi vertices och indices till format som �r redo f�r att bli konverterade till vertex och index buffer.
			//Write the mesh data to the opened package file
			//Write a chunk header to the file
			//Write a Mesh header to the file
			//Write the data to the file
		}
		
	}

	packageFile.close();
	if (packageFile.fail())
	{
		assert(false); // TODO: Warn user instead
	};

	//Zip the package file.

	//Return the filepath to the zip.
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
