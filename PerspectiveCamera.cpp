#include "pch.h"
#include "PerspectiveCamera.h"
#include "Window.h"

PerspectiveCamera::PerspectiveCamera() noexcept
{
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, viewMatrix);

	float aspectRatio = static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight());
	DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveLH(DirectX::XMConvertToRadians(45.0f), aspectRatio, 0.1f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_PerspectiveMatrix, perspectiveMatrix);

	DirectX::XMStoreFloat4x4(&m_ViewPerspectiveMatrix, viewMatrix * perspectiveMatrix);
}
