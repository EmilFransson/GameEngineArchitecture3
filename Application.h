#pragma once
#include "Window.h"
#include "UI.h"
#include "Profiler.h"
#include "Texture.h"
#include "Shader.h"
#include "InputLayout.h"
#include "Quad.h"
#include "Viewport.h"
#include "ConstantBuffer.h"
#include "PerspectiveCamera.h"

#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
#define PROFILE_FUNC Profiler TOKENPASTE2(profiler, __LINE__) (__FUNCTION__, [&](ProfileMetrics profileMetrics) {m_ProfileMetrics.push_back(std::move(profileMetrics));})
#define PROFILE_SCOPE(scopeName) Profiler TOKENPASTE2(profiler, __LINE__) (scopeName, [&](ProfileMetrics profileMetrics) {m_ProfileMetrics.push_back(std::move(profileMetrics));})

struct Transform
{
	DirectX::XMMATRIX wvpMatrix;
};

class Application
{
public:
	Application() noexcept;
	~Application() noexcept = default;
	void Run() noexcept;
private:
	void DisplayProfilingResults() noexcept;
	void GetPackagePath() noexcept;
private:
	bool m_Running;
	std::unique_ptr<UI> m_pImGui;
	std::vector<ProfileMetrics> m_ProfileMetrics;
	std::unique_ptr<VertexShader> m_pVertexShader;
	std::unique_ptr<PixelShader> m_pPixelShader;
	std::unique_ptr<InputLayout> m_pInputLayout;
	std::shared_ptr<Texture2D> m_pBrickTexture;
	std::unique_ptr<Quad> m_pQuad;
	std::unique_ptr<Viewport> m_pViewport;
	std::unique_ptr<PerspectiveCamera> m_pCamera;
	std::unique_ptr<ConstantBuffer> m_pConstantBuffer;
};