#include "DirectX11Shader.h"
#include "DirectX11Texture.h"


DirectX11Shader::DirectX11Shader(ID3D11DeviceContext* ContextIn, ID3D11VertexShader* VertexShaderIn, ID3D11PixelShader* PixelShaderIn, ID3D11InputLayout* InputLayoutIn, ITexture* TextureIn) : IShader(TextureIn), Context(ContextIn), VertexShader(VertexShaderIn), PixelShader(PixelShaderIn), InputLayout(InputLayoutIn) 
{
}

DirectX11Shader::~DirectX11Shader()
{
	if (InputLayout)
	{
		InputLayout->Release();
	}

	if (VertexShader)
	{
		VertexShader->Release();
	}

	if (PixelShader)
	{
		PixelShader->Release();
	}
}

void DirectX11Shader::Update()
{
	if (Context)
	{
		Context->IASetInputLayout(InputLayout);
		Context->VSSetShader(VertexShader, 0, 0);
		Context->PSSetShader(PixelShader, 0, 0);

		if (Texture)
		{
			Texture->Update();
		}
	}
}