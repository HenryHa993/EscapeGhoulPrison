
#pragma once

#include "Engine/IShader.h"
#include <d3d11.h>
#include <DirectXMath.h>

class DirectX11Graphics;

class DirectX11Shader : public IShader
{
public:

	DirectX11Shader(ID3D11DeviceContext* ContextIn, ID3D11VertexShader* VertexShaderIn,	ID3D11PixelShader* PixelShaderIn, ID3D11InputLayout* InputLayoutIn, ITexture* TextureIn);
	virtual ~DirectX11Shader();
	virtual void Update();

private:

	ID3D11DeviceContext* Context;
	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;
	ID3D11InputLayout* InputLayout;
};

