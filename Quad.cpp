#include "pch.h"
#include "Quad.h"
#include "Graphics.h"
#include "DXDebug.h"
#include "Utility.h"

Quad::Quad() noexcept
{
	m_Vertices.at(0).position = DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f);
	m_Vertices.at(0).texCoords = DirectX::XMFLOAT2(0.0f, 1.0f);

	m_Vertices.at(1).position = DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f);
	m_Vertices.at(1).texCoords = DirectX::XMFLOAT2(0.0f, 0.0f);

	m_Vertices.at(2).position = DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f);
	m_Vertices.at(2).texCoords = DirectX::XMFLOAT2(1.0f, 0.0f);

	m_Vertices.at(3).position = DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f);
	m_Vertices.at(3).texCoords = DirectX::XMFLOAT2(1.0f, 1.0f);

	D3D11_BUFFER_DESC vertexBufferDescriptor{};
	vertexBufferDescriptor.ByteWidth = sizeof(Vertex) * 4;
	vertexBufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	vertexBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDescriptor.CPUAccessFlags = 0u;
	vertexBufferDescriptor.MiscFlags = 0u;
	vertexBufferDescriptor.StructureByteStride = sizeof(Vertex);
	
	D3D11_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pSysMem = m_Vertices.data();

	HR_I(Graphics::GetDevice()->CreateBuffer(&vertexBufferDescriptor, &subResourceData, &m_pVertexBuffer));

	m_Indices.at(0) = 0;
	m_Indices.at(1) = 1;
	m_Indices.at(2) = 2;
	m_Indices.at(3) = 0;
	m_Indices.at(4) = 2;
	m_Indices.at(5) = 3;

	D3D11_BUFFER_DESC indexBufferDescriptor{};
	indexBufferDescriptor.ByteWidth = sizeof(unsigned short) * 6;
	indexBufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
	indexBufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
	indexBufferDescriptor.CPUAccessFlags = 0u;
	indexBufferDescriptor.MiscFlags = 0u;
	indexBufferDescriptor.StructureByteStride = sizeof(unsigned short);

	subResourceData = {};
	subResourceData.pSysMem = m_Indices.data();

	HR_I(Graphics::GetDevice()->CreateBuffer(&indexBufferDescriptor, &subResourceData, &m_pIndexBuffer));
}

void Quad::BindInternals() noexcept
{
	static const UINT stride = sizeof(Vertex);
	static const UINT offset = 0u;
	CHECK_STD(Graphics::GetContext()->IASetVertexBuffers(0u, 1u, m_pVertexBuffer.GetAddressOf(), &stride, &offset));

	CHECK_STD(Graphics::GetContext()->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0u));
}
