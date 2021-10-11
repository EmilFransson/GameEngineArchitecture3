#include "pch.h"
#include "Texture.h"
#include "Graphics.h"
#include "DXDebug.h"
#include "ResourceManager.h"
#include "Utility.h"

Texture::Texture(const uint16_t width, const uint16_t height) noexcept
	: m_pRenderTargetView{nullptr}, 
	  m_pShaderResourceView{nullptr},
	  m_Width{width},
	  m_Height{height}
{
}

Texture2D::Texture2D(const uint16_t width, const uint16_t height, const uint16_t rowPitch, void* pData) noexcept
	: Texture{width, height}, 
	  m_pTexture2D {nullptr},
	  m_pSamplerState{nullptr}
{
	D3D11_TEXTURE2D_DESC textureDescriptor{};
	textureDescriptor.Width = width;
	textureDescriptor.Height = height;
	textureDescriptor.MipLevels = 1u;
	textureDescriptor.ArraySize = 1u;
	textureDescriptor.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDescriptor.SampleDesc.Count = 1u;
	textureDescriptor.SampleDesc.Quality = 0u;
	textureDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT; // Potentially temporary.
	textureDescriptor.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDescriptor.CPUAccessFlags = 0u;
	textureDescriptor.MiscFlags = 0u;
	D3D11_SUBRESOURCE_DATA subResourceData{};
	subResourceData.pSysMem = pData;
	subResourceData.SysMemPitch = rowPitch;
	HR_I(Graphics::GetDevice()->CreateTexture2D(&textureDescriptor, &subResourceData, &m_pTexture2D));

	D3D11_RENDER_TARGET_VIEW_DESC RTVDesc{};
	RTVDesc.Format = textureDescriptor.Format;
	RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	RTVDesc.Texture2D.MipSlice = 0u;
	HR_I(Graphics::GetDevice()->CreateRenderTargetView(m_pTexture2D.Get(), &RTVDesc, &m_pRenderTargetView));

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = textureDescriptor.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1u;
	SRVDesc.Texture2D.MostDetailedMip = 0u;

	HR_I(Graphics::GetDevice()->CreateShaderResourceView(m_pTexture2D.Get(), &SRVDesc, &m_pShaderResourceView));

	D3D11_SAMPLER_DESC samplerDescriptor{};
	samplerDescriptor.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDescriptor.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescriptor.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescriptor.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDescriptor.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDescriptor.MinLOD = 0.0f;
	samplerDescriptor.MaxLOD = D3D11_FLOAT32_MAX;
	HR_I(Graphics::GetDevice()->CreateSamplerState(&samplerDescriptor, &m_pSamplerState));
}

std::shared_ptr<Texture2D> Texture2D::Create(const std::string_view filePath) noexcept
{
	return ResourceManager::Get().Load<Texture2D>(filePath);
}

void Texture2D::BindAsRenderTarget() noexcept
{
	CHECK_STD(Graphics::GetContext()->OMSetRenderTargets(1u, m_pRenderTargetView.GetAddressOf(), Graphics::GetDepthStencilView().Get()));
}

//Only supports PS as of now [Emil F]
void Texture2D::BindAsShaderResource(const uint8_t slot) noexcept
{
	CHECK_STD(Graphics::GetContext()->PSSetShaderResources(slot, 1u, m_pShaderResourceView.GetAddressOf()));
	CHECK_STD(Graphics::GetContext()->PSSetSamplers(slot, 1u, m_pSamplerState.GetAddressOf()));
}
