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
			//Write the texture to the opened package file.
			//Write a chunk header to the file
			//Write a texture header to the file
			//Write the data to the file
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
			//Först så gör vi vertices och indices till format som är redo för att bli konverterade till vertex och index buffer.
			//Write the mesh data to the opened package file
			//Write a chunk header to the file
			//Write a Mesh header to the file
			//Write the data to the file
		}
		
	}

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
