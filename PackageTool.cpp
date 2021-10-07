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

		}

		//Komprimera loader.LoadedMaterials och lägg till i package format.
		for (objl::Material currentMat : loader.LoadedMaterials)
		{

		}
		//Komprimera loader.LoadedMeshes och lägg till i package format.
		//Först så gör vi vertices och indices till format som är redo för att bli konverterade till vertex och index buffer.
		for (objl::Mesh currentMesh : loader.LoadedMeshes)
		{

		}
		
	}
}

float* PackageTool::ParseObj(const std::string& filePath)
{
	std::ifstream objFile(filePath);
	if (objFile.is_open())
	{
		std::string line;
		while (getline(objFile, line))
		{

			if (line[0] == 'v')
			{

			}
		}
	}
	else
	{
		assert(false);
	}
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
