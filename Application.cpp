#include "pch.h"
#include "Application.h"
#include "System.h"
#include "RenderCommand.h"
#include "Texture.h"

Application::Application() noexcept
	: m_Running{true}
{
	System::Initialize();
	//Default 1280 x 720 window, see function-parameters for dimensions.
	Window::Initialize(L"GameEngineArchitecture");
	m_pImGui = std::make_unique<UI>();

	m_pQuad = std::make_unique<Quad>();
	m_pQuad->BindInternals();
	m_pVertexShader = std::make_unique<VertexShader>("Shaders/VertexShader.hlsl");
	m_pPixelShader = std::make_unique<PixelShader>("Shaders/PixelShader.hlsl");
	m_pInputLayout = std::make_unique<InputLayout>(m_pVertexShader->GetVertexShaderBlob());
	m_pVertexShader->Bind();
	m_pPixelShader->Bind();
	m_pInputLayout->Bind();
	m_pBrickTexture = Texture2D::Create("brick.png");
	m_pBrickTexture->BindAsShaderResource();
	RenderCommand::SetTopolopy(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pViewport = std::make_unique<Viewport>();
	m_pViewport->Bind();

	m_pCamera = std::make_unique<PerspectiveCamera>();

	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 0.0f) * DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(0.0f)) * DirectX::XMMatrixTranslation(0.0f, 0.0f, -0.8f);
	DirectX::XMMATRIX viewPerspectiveMatrix = DirectX::XMLoadFloat4x4(&m_pCamera->GetViewProjectionMatrix());
	Transform transform;
	transform.wvpMatrix = DirectX::XMMatrixTranspose(worldMatrix * viewPerspectiveMatrix);
	m_pConstantBuffer = std::make_unique<ConstantBuffer>(static_cast<UINT>(sizeof(Transform)), 0, &transform);
	m_pConstantBuffer->BindToVertexShader();
}

void Application::Run() noexcept
{
	while (m_Running)
	{
		static const FLOAT color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		RenderCommand::ClearBackBuffer(color);
		RenderCommand::ClearDepthBuffer();
		RenderCommand::BindBackBuffer();

		RenderCommand::DrawIndexed(m_pQuad->GetNrOfindices());

		UI::Begin();
		// Windows not part of the dock space goes here:

		//...And ends here.
		UI::BeginDockSpace();
		//Windows part of the dock space goes here:
		

		//Can be reinstated if we'd need it for any assignment3-profiling.
		//DisplayProfilingResults();
		//...And ends here.
		UI::EndDockSpace();
		//No UI-windows in this part and after!
		UI::End();

		RenderCommand::UnbindRenderTargets();

		if (!Window::OnUpdate())
		{
			m_Running = false;
		}
	}
}

void Application::DisplayProfilingResults() noexcept
{
	ImGui::Begin("Profiling metrics");
	for (auto& metric : m_ProfileMetrics)
	{
		ImGui::Text(std::to_string(metric.Duration).c_str());
		ImGui::SameLine();
		ImGui::Text("ms.");
		ImGui::SameLine();
		ImGui::Text(metric.Name.c_str());
	}
	ImGui::End();
	m_ProfileMetrics.clear();
}