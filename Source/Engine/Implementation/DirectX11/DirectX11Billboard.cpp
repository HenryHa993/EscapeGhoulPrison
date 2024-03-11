#include "DirectX11Billboard.h"

#include <D3DCompiler.h>
#include "DirectX11Graphics.h"

DirectX11Billboard::DirectX11Billboard(ID3D11DeviceContext* ContextIn, ID3D11Buffer* VertexBufferIn, unsigned int vertexStride, unsigned int vertexOffset, unsigned int vertexCountIn) : Context(ContextIn), VertexBuffer(VertexBufferIn), vertexStride(vertexStride), vertexOffset(vertexOffset), vertexCount(vertexCountIn)
{
	IsActive = true;
}

DirectX11Billboard::~DirectX11Billboard()
{
	if (VertexBuffer)
	{
		VertexBuffer->Release();
	}
}
	
void DirectX11Billboard::Update()
{
	if(!IsActive)
	{
		return;
	}

	if (Context)
	{
		Context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context->IASetVertexBuffers(0, 1, &VertexBuffer, &vertexStride, &vertexOffset);
		Context->Draw(vertexCount, 0);
	}
}

//void DirectX11Billboard::SetActive(bool b)
//{
//	IsActive = b;
//}
