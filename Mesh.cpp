#include "pch.h"
#include "Mesh.h"
#include "Graphics.h"
#include "DXDebug.h"
#include "Utility.h"
#include "ResourceManager.h"

MeshOBJ::MeshOBJ(const std::vector<objl::Vertex>& vertices, const std::vector<unsigned int> indices) noexcept
	: m_NrOfIndices{ indices.size() }, m_Strides{ sizeof(objl::Vertex)}
{
	D3D11_BUFFER_DESC vertexBufferDescriptor{};
	vertexBufferDescriptor.ByteWidth = sizeof(objl::Vertex) * static_cast<UINT>(vertices.size());
	vertexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0u;
	vertexBufferDescriptor.MiscFlags = 0u;
	vertexBufferDescriptor.StructureByteStride = sizeof(objl::Vertex);

	D3D11_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pSysMem = vertices.data();

	HR_I(Graphics::GetDevice()->CreateBuffer(&vertexBufferDescriptor, &subResourceData, &m_pVertexBuffer));

	D3D11_BUFFER_DESC indexBufferDescriptor{};
	indexBufferDescriptor.ByteWidth = sizeof(unsigned int) * static_cast<UINT>(indices.size());
	indexBufferDescriptor.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0u;
	indexBufferDescriptor.MiscFlags = 0u;
	indexBufferDescriptor.StructureByteStride = sizeof(unsigned int);

	D3D11_SUBRESOURCE_DATA subResourceData2{};
	subResourceData2.pSysMem = indices.data();

	HR_I(Graphics::GetDevice()->CreateBuffer(&indexBufferDescriptor, &subResourceData2, &m_pIndexBuffer));
}

void MeshOBJ::BindInternals(const uint8_t slot) noexcept
{
	static const UINT offset = 0u;
	CHECK_STD(Graphics::GetContext()->IASetVertexBuffers(slot, 1u, m_pVertexBuffer.GetAddressOf(), &m_Strides, &offset));
	CHECK_STD(Graphics::GetContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0u));
}

std::vector<std::shared_ptr<MeshOBJ>> MeshOBJ::Create(const std::string& fileName) noexcept
{
	return ResourceManager::Get().LoadMultiple<MeshOBJ>(fileName);
}

