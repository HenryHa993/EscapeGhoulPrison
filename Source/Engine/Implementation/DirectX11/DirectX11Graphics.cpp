#include "DirectX11Graphics.h"

#include "DirectX11Billboard.h"
#include "DirectX11Shader.h"
#include "DirectX11Texture.h"
#include <d3dcommon.h>
#include <d3d11.h>
#include <D3DCompiler.h>
#include <DDSTextureLoader.h>

DirectX11Graphics::DirectX11Graphics(HWND hwndIn) : Device(nullptr), Context(nullptr), SwapChain(nullptr), BackbufferView(nullptr), BackbufferTexture(nullptr), Mvp(nullptr), vpMatrix(), FeatureLevel(D3D_FEATURE_LEVEL_11_0), hwnd(hwndIn), width(0), height(0)
{
    RECT dimensions;
    GetClientRect(hwnd, &dimensions);
    width = dimensions.right - dimensions.left;
    height = dimensions.bottom - dimensions.top;

    D3D_DRIVER_TYPE driverTypes[] = { D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE };
    unsigned int totalDriverTypes = ARRAYSIZE(driverTypes);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    HRESULT hr = S_OK;
    unsigned int creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    for (unsigned int count = 0; count < totalDriverTypes; ++count)
    {
        hr = D3D11CreateDeviceAndSwapChain(NULL, driverTypes[count], NULL, creationFlags, NULL, 0, D3D11_SDK_VERSION, &sd, &SwapChain, &Device, &FeatureLevel, &Context);

        if (SUCCEEDED(hr))
        {
            break;
        }
    }

    if (SUCCEEDED(hr))
    {
        hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackbufferTexture);
    }

    if (SUCCEEDED(hr))
    {
        hr = Device->CreateRenderTargetView(BackbufferTexture, 0, &BackbufferView);
    }

    if (BackbufferTexture)
    {
        BackbufferTexture->Release();
    }

    if (FAILED(hr))
    {
        MessageBox(NULL, "Graphics Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
    }
    else
    {
        D3D11_BUFFER_DESC constDesc;
        ZeroMemory(&constDesc, sizeof(constDesc));
        constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        constDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
        constDesc.Usage = D3D11_USAGE_DEFAULT;
        hr = Device->CreateBuffer(&constDesc, 0, &Mvp);

        if (FAILED(hr))
        {
            MessageBox(NULL, "Graphics Failed to create MVP Buffer", "Error!", MB_ICONEXCLAMATION | MB_OK);
        }

        float halfWidth = static_cast<float>(width / 2);
        float halfHeight = static_cast<float>(height / 2);
        DirectX::XMMATRIX view = DirectX::XMMatrixIdentity();
        DirectX::XMMATRIX projection = DirectX::XMMatrixOrthographicOffCenterLH(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 10.1f);
        vpMatrix = DirectX::XMMatrixMultiply(view, projection);

        D3D11_BLEND_DESC Desc;
        ZeroMemory(&Desc, sizeof(D3D11_BLEND_DESC));
        Desc.RenderTarget[0].BlendEnable = TRUE;
        Desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        Desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        Desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        Desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        Desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        Desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        Desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        Device->CreateBlendState(&Desc, &BlendState);
    }
}

DirectX11Graphics::~DirectX11Graphics()
{
    if (BlendState)
    {
        BlendState->Release();
    }

    if (BackbufferView)
    {
        BackbufferView->Release();
    }

    if (SwapChain)
    {
        SwapChain->Release();
    }

    if (Context)
    {
        Context->Release();
    }

    if (Device)
    {
        Device->Release();
    }
}

void DirectX11Graphics::Update()
{
    if (Context && SwapChain)
    {
        float clearColour[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
        Context->ClearRenderTargetView(BackbufferView, clearColour);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        Context->RSSetViewports(1, &viewport);

        Context->OMSetRenderTargets(1, &BackbufferView, NULL);

        for (auto bucket = Renderables.begin(); bucket != Renderables.end(); ++bucket)
        {
            bucket->first->Update();
 
            for (auto renderable = bucket->second.begin(); renderable != bucket->second.end(); ++renderable)
            {
                SetWorldMatrix((*renderable)->GetTransform());
                Context->OMSetBlendState(BlendState, NULL, ~0U);
                (*renderable)->Update();
            }
        }

        SwapChain->Present(0, 0);
    }
}

bool DirectX11Graphics::IsValid()
{
    return Device != nullptr;
}

ITexture* DirectX11Graphics::CreateTexture(const wchar_t* filepath)
{
    ITexture* Result = nullptr;
    ID3D11ShaderResourceView* Texture = nullptr;
    ID3D11SamplerState* Sampler = nullptr;
    D3D11_TEXTURE2D_DESC Description;

    if (IsValid())
    {
        HRESULT hr = DirectX::CreateDDSTextureFromFile(Device, filepath, NULL, &Texture);

        if (SUCCEEDED(hr))
        {
            D3D11_SAMPLER_DESC colorMapDesc;
            ZeroMemory(&colorMapDesc, sizeof(colorMapDesc));
            colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

            hr = Device->CreateSamplerState(&colorMapDesc, &Sampler);
        }

        if (SUCCEEDED(hr))
        {
            ID3D11Resource* textureResource;
            Texture->GetResource(&textureResource);

            ((ID3D11Texture2D*)textureResource)->GetDesc(&Description);
            textureResource->Release();
        }

        if (SUCCEEDED(hr))
        {
            Result = new DirectX11Texture(Context, Texture, Sampler, Description);
            Textures.push_back(Result);
        }
    }

    return Result;
}

IShader* DirectX11Graphics::CreateShader(const wchar_t* filepath, const char* vsentry, const char* vsshader, const char* psentry, const char* psshader, ITexture* TextureIn)
{
    IShader* Result = nullptr;
    ID3D11VertexShader* VertexShader = nullptr;
    ID3D11PixelShader* PixelShader = nullptr;
    ID3D11InputLayout* InputLayout = nullptr;
    HRESULT hr = S_FALSE;
    ID3DBlob* vsBuffer = 0;

    if (IsValid())
    {
        if (CompileShader(filepath, vsentry, vsshader, &vsBuffer))
        {
            hr = Device->CreateVertexShader(vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), 0, &VertexShader);

            if (FAILED(hr) && VertexShader)
            {
                VertexShader->Release();
            }
            else
            {
                D3D11_INPUT_ELEMENT_DESC layout[] =
                {
                    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
                };

                unsigned int totalLayoutElements = ARRAYSIZE(layout);

                hr = Device->CreateInputLayout(layout, totalLayoutElements, vsBuffer->GetBufferPointer(), vsBuffer->GetBufferSize(), &InputLayout);
                vsBuffer->Release();
            }
        }

        if (SUCCEEDED(hr))
        {
            ID3DBlob* psBuffer = 0;
            hr = S_FALSE;
            if (CompileShader(filepath, psentry, psshader, &psBuffer))
            {
                hr = Device->CreatePixelShader(psBuffer->GetBufferPointer(), psBuffer->GetBufferSize(), 0, &PixelShader);
                psBuffer->Release();
            }
        }

        if (SUCCEEDED(hr))
        {
            Result = new DirectX11Shader(Context, VertexShader, PixelShader, InputLayout, TextureIn);
            Renderables.insert(std::pair<IShader*, std::list<IRenderable*> >(Result, std::list<IRenderable*>()));
        }
    }

    return Result;
}

IRenderable* DirectX11Graphics::CreateBillboard(IShader* ShaderIn)
{
    IRenderable* Result = nullptr;

    if (IsValid())
    {
        const ITexture* texture = ShaderIn->GetTexture();
        const float halfWidth = texture ? texture->GetWidth() / 2.0f : 0.5f;
        const float halfHeight = texture ? texture->GetHeight() / 2.0f : 0.5f;

        float vertex_data_array[] =
        {
            halfWidth,  halfHeight, 0.0f,  1.0f, 1.0f,
            halfWidth, -halfHeight, 0.0f,  1.0f, 0.0f,
           -halfWidth, -halfHeight, 0.0f,  0.0f, 0.0f,

           -halfWidth, -halfHeight, 0.0f,  0.0f, 0.0f,
           -halfWidth,  halfHeight, 0.0f,  0.0f, 1.0f,
            halfWidth,  halfHeight, 0.0f,  1.0f, 1.0f,
        };

        ID3D11Buffer* VertexBuffer;
        unsigned int vertexStride = 5 * sizeof(float);
        unsigned int vertexOffset = 0;
        unsigned int vertexCount = 6;

        D3D11_BUFFER_DESC vertexDescription;
        ZeroMemory(&vertexDescription, sizeof(vertexDescription));
        vertexDescription.Usage = D3D11_USAGE_DEFAULT;
        vertexDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexDescription.ByteWidth = sizeof(vertex_data_array);

        D3D11_SUBRESOURCE_DATA resourceData;
        ZeroMemory(&resourceData, sizeof(resourceData));
        resourceData.pSysMem = vertex_data_array;

        if (SUCCEEDED(Device->CreateBuffer(&vertexDescription, &resourceData, &VertexBuffer)))
        {
            Result = new DirectX11Billboard(Context, VertexBuffer, vertexStride, vertexOffset, vertexCount);
            Renderables[ShaderIn].push_back(Result);
        }
    }

    return Result;
}

void DirectX11Graphics::SetWorldMatrix(const Transform2D& transform)
{
    DirectX::XMMATRIX translation = DirectX::XMMatrixTranslation(transform.PositionX, transform.PositionY, 10.0f);
    DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationZ(transform.Rotation);
    DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(transform.ScaleX, transform.ScaleY, 1.0f);
    DirectX::XMMATRIX world = scale * rotation * translation;
    DirectX::XMMATRIX mvp = DirectX::XMMatrixMultiply(world, vpMatrix);
    mvp = DirectX::XMMatrixTranspose(mvp);
    Context->UpdateSubresource(Mvp, 0, 0, &mvp, 0, 0);
    Context->VSSetConstantBuffers(0, 1, &Mvp);
}

bool DirectX11Graphics::CompileShader(LPCWSTR filepath, LPCSTR entry, LPCSTR shader, ID3DBlob** buffer)
{
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#ifdef _DEBUG
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBuffer = 0;
    HRESULT hr = D3DCompileFromFile(filepath, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry, shader, shaderFlags, 0, buffer, &errorBuffer);

    if (FAILED(hr))
    {
        if (errorBuffer)
        {
            MessageBox(NULL, (char*)errorBuffer->GetBufferPointer(), "Error!", MB_ICONEXCLAMATION | MB_OK);
        }
    }

    if (errorBuffer)
    {
        errorBuffer->Release();
    }

    return hr == S_OK;
}
