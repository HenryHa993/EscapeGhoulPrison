#include "IShader.h"
#include "ITexture.h"

IShader::IShader(ITexture* TextureIn) : Texture(TextureIn)
{
}

const ITexture* IShader::GetTexture() const
{
    return Texture;
}
