#pragma once
#include "Resource.h"
class Mesh : public Resource
{
public:
	Mesh() noexcept;
	virtual ~Mesh() noexcept override = default;
	virtual void BindInternals(const uint8_t slot = 0u) noexcept = 0;
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	uint32_t m_NrOfIndices;
	uint32_t m_Strides;
};

class MeshOBJ : public Mesh 
{
public:
	MeshOBJ() noexcept = default;
	virtual ~MeshOBJ() noexcept override = default;
	virtual void BindInternals(const uint8_t slot = 0u) noexcept override;
	[[nodiscard]] static std::shared_ptr<MeshOBJ> Create(const std::string& fileName) noexcept;
};