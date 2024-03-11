
#pragma once

class ITexture;

class IShader
{
public:

	IShader(ITexture* TextureIn);
	virtual void Update() = 0;

	const ITexture* GetTexture() const;

protected: 

	ITexture* Texture;

};

