
#pragma once

#include "Engine/ITexture.h"
#include <d3d11.h>

class DirectX11Texture : public ITexture
{
public:

	DirectX11Texture(ID3D11DeviceContext* ContextIn, ID3D11ShaderResourceView* TextureIn, ID3D11SamplerState* SamplerIn, const D3D11_TEXTURE2D_DESC& DescriptionIn);
	virtual ~DirectX11Texture();

	virtual void Update();
	virtual float GetWidth() const;
	virtual float GetHeight() const;

private:

	ID3D11DeviceContext* Context;
	ID3D11ShaderResourceView* Texture;
	ID3D11SamplerState* Sampler;
	D3D11_TEXTURE2D_DESC Description;
};

