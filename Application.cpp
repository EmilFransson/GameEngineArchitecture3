#include "pch.h"
#include "Application.h"
#include "System.h"
#include "RenderCommand.h"

Application::Application() noexcept
	: m_Running{true}
{
	System::Initialize();
	//Default 1280 x 720 window, see function-parameters for dimensions.
	Window::Initialize(L"GameEngineArchitecture");
	m_pImGui = std::make_unique<UI>();
}

void Application::Run() noexcept
{
	while (m_Running)
	{
		static const FLOAT color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		RenderCommand::ClearBackBuffer(color);
		RenderCommand::ClearDepthBuffer();
		RenderCommand::BindBackBuffer();

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