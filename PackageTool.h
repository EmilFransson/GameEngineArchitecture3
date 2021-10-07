#pragma once

#include "OBJ_Loader.h"
class PackageTool
{
private:
public:
	struct PackageHeader
	{
		char id[3]; // PCK or PKG or whatever, not strictly necessary but feels official
		uint8_t assetCount;
		uint32_t size;
	};

	struct ChunkHeader
	{
		char type[4]; // Ex: TEX, MESH, AUD ...
		uint32_t chunkSize; // Chunk header not included
		uint32_t GUID; //Generated by hashing the compressed data.
		std::string humanReadable; //A readable name for the resource that is connected to the GUID, probably will be the name of the file.
	};

	struct TextureHeader
	{
		char textureType[4]; // COL, NORM, SPEC ...
		uint32_t dataSize;
		uint16_t width, height;
		uint16_t rowPitch;

		uint16_t pad;
	};

	struct MeshHeader
	{
		std::string materialName;
		uint32_t verticesDataSize; //The size of the vertexdata part of the data.
		uint32_t indicesDataSize; //The size of the indexdata part of the data.
	};
	
	struct MaterialHeader
	{
		uint32_t dataSize;
	};
private:
public:
	PackageTool() = default;
	~PackageTool() = default;

	void Package(const std::string& dirPath);
private:
	float* ParseObj(const std::string& dirPath);
};
