#include "DirectX11Texture.h"

DirectX11Texture::DirectX11Texture(ID3D11DeviceContext* ContextIn, ID3D11ShaderResourceView* TextureIn, ID3D11SamplerState* SamplerIn, const D3D11_TEXTURE2D_DESC& DescriptionIn) : Context(ContextIn), Texture(TextureIn), Sampler(SamplerIn), Description(DescriptionIn)
{
}

DirectX11Texture::~DirectX11Texture()
{
	if (Texture)
	{
		Texture->Release();
	}

	if (Sampler)
	{
		Sampler->Release();
	}
}

void DirectX11Texture::Update()
{
	if (Context)
	{
		Context->PSSetShaderResources(0, 1, &Texture);
		Context->PSSetSamplers(0, 1, &Sampler);
	}
}

float DirectX11Texture::GetWidth() const
{
	return static_cast<float>(Description.Width);
}

float DirectX11Texture::GetHeight() const
{
	return static_cast<float>(Description.Height);
}
