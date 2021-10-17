#pragma once
#include "Texture.h"
#include "Mesh.h"

class Model
{
public:
	Model() noexcept = default;
	~Model() noexcept = default;
private:
	std::vector<std::shared_ptr<MeshOBJ>> m_pMeshes;
	std::shared_ptr<Texture2D> m_pTexture;
};