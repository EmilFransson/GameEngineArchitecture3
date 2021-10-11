#pragma once
class PerspectiveCamera
{
public:
	PerspectiveCamera() noexcept;
	~PerspectiveCamera() noexcept = default;
	[[nodiscard]] constexpr DirectX::XMFLOAT4X4& GetViewProjectionMatrix() noexcept { return m_ViewPerspectiveMatrix; }
private:
	DirectX::XMFLOAT4X4 m_ViewMatrix;
	DirectX::XMFLOAT4X4 m_PerspectiveMatrix;
	DirectX::XMFLOAT4X4 m_ViewPerspectiveMatrix;
};