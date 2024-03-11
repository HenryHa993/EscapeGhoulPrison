#include "IApplication.h"
#include "IGraphics.h"
#include "IInput.h"

IApplication::IApplication(IGraphics* GraphicsIn, IInput* InputIn) : Graphics(GraphicsIn), Input(InputIn)
{
}

IApplication::~IApplication()
{
}
