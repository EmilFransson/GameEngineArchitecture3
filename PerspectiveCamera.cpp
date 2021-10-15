#include "pch.h"
#include "PerspectiveCamera.h"
#include "Window.h"

PerspectiveCamera::PerspectiveCamera() noexcept :
	m_pos{ 0.0f, 0.0f, -5.0f },
	m_rot{ 0.0f, 0.0f, 0.0f },
	m_forward{ 0.0f, 0.0f, 1.0f },
	m_up{ 0.0f, 1.0f, 0.0f },
	m_right{ 1.0f, 0.0f, 0.0f },
	m_speed{ 20.0f }
{
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_pos), DirectX::XMLoadFloat3(&m_forward), DirectX::XMLoadFloat3(&m_up));
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, viewMatrix);

	float aspectRatio = static_cast<float>(Window::GetWidth()) / static_cast<float>(Window::GetHeight());
	DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(45.0f), aspectRatio, 0.1f, 1000.0f);
	DirectX::XMStoreFloat4x4(&m_PerspectiveMatrix, perspectiveMatrix);
	DirectX::XMStoreFloat4x4(&m_ViewPerspectiveMatrix, viewMatrix * perspectiveMatrix);
}

void PerspectiveCamera::Update(float deltaTime)
{
	HandleInput(deltaTime);
	
	//Update view matrix.
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&m_pos), DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&m_pos), DirectX::XMLoadFloat3(&m_forward)), DirectX::XMLoadFloat3(&m_up));
	DirectX::XMStoreFloat4x4(&m_ViewMatrix, viewMatrix);

	DirectX::XMStoreFloat4x4(&m_ViewPerspectiveMatrix, viewMatrix * DirectX::XMLoadFloat4x4(&m_PerspectiveMatrix));
}

void PerspectiveCamera::HandleInput(float deltaTime)
{
	std::vector<int> keyMap = Window::Get().GetKeyMap();

	for (auto key : keyMap)
	{
		switch (key)
		{
		case 87: //w
		{
			m_pos.x += (deltaTime * m_forward.x * m_speed);
			m_pos.y += (deltaTime * m_forward.y * m_speed);
			m_pos.z += (deltaTime * m_forward.z * m_speed);
			break;
		}
		case 65: //a
		{
			m_pos.x -= (deltaTime * m_right.x * m_speed);
			m_pos.y -= (deltaTime * m_right.y * m_speed);
			m_pos.z -= (deltaTime * m_right.z * m_speed);
			break;
		}
		case 83: //s
		{
			m_pos.x -= (deltaTime * m_forward.x * m_speed);
			m_pos.y -= (deltaTime * m_forward.y * m_speed);
			m_pos.z -= (deltaTime * m_forward.z * m_speed);
			break;
		}
		case 68: //d
		{
			m_pos.x += (deltaTime * m_right.x * m_speed);
			m_pos.y += (deltaTime * m_right.y * m_speed);
			m_pos.z += (deltaTime * m_right.z * m_speed);
			break;
		}
		default:
		{
			break;
		}
		}
	}
}