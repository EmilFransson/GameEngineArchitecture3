#pragma once
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

	};
private:
public:
	PackageTool() = default;
	~PackageTool() = default;

	void Package(const std::string& dirPath);
private:
	float* ParseObj(const std::string& dirPath);
};

