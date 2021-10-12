#pragma once
#include "Resource.h"
class Texture : public Resource
{
public:
	Texture(const uint32_t width, const uint32_t height) noexcept;
	virtual ~Texture() noexcept override = default;
	virtual void BindAsRenderTarget() noexcept = 0;
	virtual void BindAsShaderResource(const uint8_t slot = 0u) noexcept = 0;
protected:
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pShaderResourceView;
	uint32_t m_Width;
	uint32_t m_Height;
};

class Texture2D : public Texture 
{
public:
	Texture2D(const uint32_t width, const uint32_t height, const uint32_t rowPitch, void* pData) noexcept;
	virtual ~Texture2D() noexcept override = default;
	[[nodiscard]] static std::shared_ptr<Texture2D> Create(const std::string_view filePath) noexcept;
	virtual void BindAsRenderTarget() noexcept override;
	virtual void BindAsShaderResource(const uint8_t slot = 0u) noexcept override;
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_pTexture2D;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSamplerState;
};