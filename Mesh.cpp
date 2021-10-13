#include "pch.h"
#include "Mesh.h"
#include "Graphics.h"
#include "DXDebug.h"
#include "Utility.h"
#include "ResourceManager.h"

Mesh::Mesh() noexcept
	: m_pVertexBuffer{nullptr},
	  m_pIndexBuffer{nullptr},
	  m_NrOfIndices{0u},
	  m_Strides{0u}
{
}

void MeshOBJ::BindInternals(const uint8_t slot) noexcept
{
	static const UINT offset = 0u;
	CHECK_STD(Graphics::GetContext()->IASetVertexBuffers(slot, 1u, m_pVertexBuffer.GetAddressOf(), &m_Strides, &offset));
	CHECK_STD(Graphics::GetContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0u));
}

std::shared_ptr<MeshOBJ> MeshOBJ::Create(const std::string& fileName) noexcept
{
	return ResourceManager::Get().Load<MeshOBJ>(fileName);
}
