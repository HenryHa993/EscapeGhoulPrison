#include "IGraphics.h"
#include "ITexture.h"
#include "IShader.h"
#include "IRenderable.h"


IGraphics::IGraphics() : Renderables()
{
}

IGraphics::~IGraphics()
{
    for (auto bucket = Renderables.begin(); bucket != Renderables.end(); ++bucket)
    {
        for (auto renderable = bucket->second.begin(); renderable != bucket->second.end(); ++renderable)
        {
            delete (*renderable);
        }
        
        delete bucket->first;
    }

    Renderables.clear();

    for (auto texture = Textures.begin(); texture != Textures.end(); ++texture)
    {
        delete (*texture);
    }

    Textures.clear();
}
