#include "pch.h"
#include "PackageTool.h"

void PackageTool::Package(const std::string& dirPath)
{
	std::filesystem::directory_entry Folder = std::filesystem::directory_entry(dirPath);
	if (!Folder.is_directory())
	{
		assert(false);
	}

	for (auto const& dir_entry : std::filesystem::directory_iterator{dirPath})
	{
		if (!dir_entry.path().has_extension())
		{
			assert(false);
		}
		
		std::string filetype = dir_entry.path().extension().string();
		
		if (filetype == ".obj")
		{
			ParseObj(dir_entry.path().string());
		}
		else if (filetype == ".png")
		{

		}
	}
}

float* PackageTool::ParseObj(const std::string& filePath)
{
	std::ifstream objFile(filePath);
	if (objFile.is_open())
	{

	}
	else
	{

	}
}