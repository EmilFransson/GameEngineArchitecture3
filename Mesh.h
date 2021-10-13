#pragma once
#include "Resource.h"
#include "OBJ_Loader.h"
class Mesh : public Resource
{
public:
	Mesh() noexcept = default;
	virtual ~Mesh() noexcept override = default;
	virtual void BindInternals(const uint8_t slot = 0u) noexcept = 0;
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
};

class MeshOBJ : public Mesh 
{
public:
	MeshOBJ(const std::vector<objl::Vertex>& vertices, const std::vector<unsigned int> indices) noexcept;
	virtual ~MeshOBJ() noexcept override = default;
	virtual void BindInternals(const uint8_t slot = 0u) noexcept override;
	[[nodiscard]] static std::shared_ptr<MeshOBJ> Create(const std::string& fileName) noexcept;
private:
	uint64_t m_NrOfIndices;
	UINT m_Strides;
};