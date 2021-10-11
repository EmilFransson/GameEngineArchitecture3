#pragma once
#include "Vertex.h"
class Quad
{
public:
	Quad() noexcept;
	~Quad() noexcept = default;
	void BindInternals() noexcept;
	[[nodiscard]] constexpr uint32_t GetNrOfindices() const noexcept { return static_cast<uint32_t>(m_Indices.size()); }
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;
	std::array<Vertex, 4> m_Vertices;
	std::array<unsigned short, 6> m_Indices;
};