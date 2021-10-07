#include "pch.h"
#include "PackageTool.h"
#include "stb_image.h"

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
