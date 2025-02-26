#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <chrono>
#include "DDSTextureLoader.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;

// Global variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* m_pDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
ID3D11DepthStencilView* g_pDepthStencilView = nullptr;
const float g_ClearColor[4] = {0.2f, 0.2f, 0.4f, 1.0f};

// Buffers and shaders
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;
ID3D11Buffer* g_pVertexBuffer = nullptr;

// Constant buffers
ID3D11Buffer* g_pModelBuffer = nullptr;
ID3D11Buffer* g_pVPBuffer = nullptr;

// Cube
ID3D11ShaderResourceView* g_pCubeTextureRV = nullptr;
ID3D11SamplerState* g_pSamplerLinear = nullptr;

// Skybox
ID3D11VertexShader* g_pSkyboxVS = nullptr;
ID3D11PixelShader* g_pSkyboxPS = nullptr;
ID3D11InputLayout* g_pSkyboxInputLayout = nullptr;
ID3D11Buffer* g_pSkyboxVertexBuffer = nullptr;
ID3D11Buffer* g_pSkyboxVPBuffer = nullptr;
ID3D11ShaderResourceView* g_pSkyboxTextureRV = nullptr;

// Camera control
float g_CubeAngle = 0.0f;
float g_CameraAngle = 0.0f;
bool   g_MouseDragging = false;
POINT  g_LastMousePos = { 0, 0 };
float  g_CameraAzimuth = 0.0f;
float  g_CameraElevation = 0.0f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitDevice(HWND hWnd);
void CleanupDevice();
void Render();
HRESULT CreateCubeResources();

struct Vertex {
    XMFLOAT3 position;
    float u, v;
};

struct SkyboxVertex {
    float x, y, z;
};

Vertex g_CubeVertices[] = {
    {XMFLOAT3(-0.5f, -0.5f, 0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, -0.5f, 0.5f), 1.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, 0.5f), 1.0f, 0.0f},

    {XMFLOAT3(-0.5f, -0.5f, 0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, 0.5f), 1.0f, 0.0f},
    {XMFLOAT3(-0.5f, 0.5f, 0.5f), 0.0f, 0.0f},

    {XMFLOAT3(0.5f, -0.5f, -0.5f), 0.0f, 1.0f},
    {XMFLOAT3(-0.5f, -0.5f, -0.5f), 1.0f, 1.0f},
    {XMFLOAT3(-0.5f, 0.5f, -0.5f), 1.0f, 0.0f},

    {XMFLOAT3(0.5f, -0.5f, -0.5f), 0.0f, 1.0f},
    {XMFLOAT3(-0.5f, 0.5f, -0.5f), 1.0f, 0.0f},
    {XMFLOAT3(0.5f, 0.5f, -0.5f), 0.0f, 0.0f},

    {XMFLOAT3(-0.5f, -0.5f, -0.5f), 0.0f, 1.0f},
    {XMFLOAT3(-0.5f, -0.5f, 0.5f), 1.0f, 1.0f},
    {XMFLOAT3(-0.5f, 0.5f, 0.5f), 1.0f, 0.0f},

    {XMFLOAT3(-0.5f, -0.5f, -0.5f), 0.0f, 1.0f},
    {XMFLOAT3(-0.5f, 0.5f, 0.5f), 1.0f, 0.0f},
    {XMFLOAT3(-0.5f, 0.5f, -0.5f), 0.0f, 0.0f},

    {XMFLOAT3(0.5f, -0.5f, 0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, -0.5f, -0.5f), 1.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, -0.5f), 1.0f, 0.0f},

    {XMFLOAT3(0.5f, -0.5f, 0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, -0.5f), 1.0f, 0.0f},
    {XMFLOAT3(0.5f, 0.5f, 0.5f), 0.0f, 0.0f},

    {XMFLOAT3(-0.5f, 0.5f, 0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, 0.5f), 1.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, -0.5f), 1.0f, 0.0f},

    {XMFLOAT3(-0.5f, 0.5f, 0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, 0.5f, -0.5f), 1.0f, 0.0f},
    {XMFLOAT3(-0.5f, 0.5f, -0.5f), 0.0f, 0.0f},

    {XMFLOAT3(-0.5f, -0.5f, -0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, -0.5f, -0.5f), 1.0f, 1.0f},
    {XMFLOAT3(0.5f, -0.5f, 0.5f), 1.0f, 0.0f},

    {XMFLOAT3(-0.5f, -0.5f, -0.5f), 0.0f, 1.0f},
    {XMFLOAT3(0.5f, -0.5f, 0.5f), 1.0f, 0.0f},
    {XMFLOAT3(-0.5f, -0.5f, 0.5f), 0.0f, 0.0f},
};

SkyboxVertex g_SkyboxVertices[] = {
    { -1.0f, -1.0f, -1.0f },
    { -1.0f, 1.0f, -1.0f },
    {  1.0f, 1.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f },
    {  1.0f, 1.0f, -1.0f },
    {  1.0f, -1.0f, -1.0f },

    {  1.0f, -1.0f, 1.0f },
    {  1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f },
    {  1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f },
    { -1.0f, -1.0f, 1.0f },

    { -1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, -1.0f },
    { -1.0f, -1.0f, 1.0f },
    { -1.0f, 1.0f, -1.0f },
    { -1.0f, -1.0f, -1.0f },

    { 1.0f, -1.0f, -1.0f },
    { 1.0f, 1.0f, -1.0f },
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f },
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, -1.0f, 1.0f },

    { -1.0f, 1.0f, -1.0f },
    { -1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, 1.0f },
    { -1.0f, 1.0f, -1.0f },
    { 1.0f, 1.0f, 1.0f },
    { 1.0f, 1.0f, -1.0f },

    { -1.0f, -1.0f, 1.0f },
    { -1.0f, -1.0f, -1.0f },
    { 1.0f, -1.0f, -1.0f },
    { -1.0f, -1.0f, 1.0f },
    { 1.0f, -1.0f, -1.0f },
    { 1.0f, -1.0f, 1.0f },
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = L"Laboratory work 4";

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(
        L"Laboratory work 4",
        L"Laboratory work 4",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        1280, 720,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );
    if (!hWnd)
        return FALSE;

    if (FAILED(InitDevice(hWnd)))
        return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = { 0 };
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            Render();
        }
    }

    CleanupDevice();
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN:
        g_MouseDragging = true;
        g_LastMousePos.x = LOWORD(lParam);
        g_LastMousePos.y = HIWORD(lParam);
        SetCapture(hWnd);
        break;
    case WM_LBUTTONUP:
        g_MouseDragging = false;
        ReleaseCapture();
        break;
    case WM_MOUSEMOVE:
        if (g_MouseDragging) {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int dx = x - g_LastMousePos.x;
            int dy = y - g_LastMousePos.y;
            g_CameraAzimuth += dx * 0.005f;
            g_CameraElevation += dy * 0.005f;
            if (g_CameraElevation > XM_PIDIV2 - 0.01f)
                g_CameraElevation = XM_PIDIV2 - 0.01f;
            if (g_CameraElevation < -XM_PIDIV2 + 0.01f)
                g_CameraElevation = -XM_PIDIV2 + 0.01f;
            g_LastMousePos.x = x;
            g_LastMousePos.y = y;
        }
        break;
    case WM_KEYDOWN:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

HRESULT InitDevice(HWND hWnd) {
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

    D3D_FEATURE_LEVEL featureLevel;
    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        &featureLevel,
        &m_pDeviceContext
    );
    if (FAILED(hr))
        return hr;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    ID3D11Texture2D* pDepthStencil = nullptr;

    hr = g_pd3dDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateDepthStencilView(pDepthStencil, nullptr, &g_pDepthStencilView);
    pDepthStencil->Release();
    if (FAILED(hr))
        return hr;

    m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    D3D11_VIEWPORT vp = {};
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = static_cast<FLOAT>(width);
    vp.Height = static_cast<FLOAT>(height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_pDeviceContext->RSSetViewports(1, &vp);

    hr = CreateCubeResources();
    if (FAILED(hr))
        return hr;

    return S_OK;
}

HRESULT CreateCubeResources() {
    HRESULT hr = S_OK;

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;

    hr = D3DCompileFromFile(
        L"vs.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_4_0",
        0,
        0,
        &vsBlob,
        nullptr
    );
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(
        L"ps.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_4_0",
        0,
        0,
        &psBlob,
        nullptr
    );
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateVertexShader(
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        nullptr,
        &g_pVertexShader
    );
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreatePixelShader(
        psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(),
        nullptr,
        &g_pPixelShader
    );
    if (FAILED(hr))
        return hr;

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, sizeof(float) * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = g_pd3dDevice->CreateInputLayout(
        layout,
        2,
        vsBlob->GetBufferPointer(),
        vsBlob->GetBufferSize(),
        &g_pVertexLayout
    );

    vsBlob->Release();
    psBlob->Release();

    // Index buffer
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(g_CubeVertices);
    ibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = g_CubeVertices;

    hr = g_pd3dDevice->CreateBuffer(&ibd, &initData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(XMMATRIX);
    ibd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ibd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&ibd, nullptr, &g_pModelBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC vpBufferDesc = {};
    vpBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vpBufferDesc.ByteWidth = sizeof(XMMATRIX);
    vpBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vpBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = g_pd3dDevice->CreateBuffer(&vpBufferDesc, nullptr, &g_pVPBuffer);
    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(g_pd3dDevice, L"cube.dds", nullptr, &g_pCubeTextureRV);
    if (FAILED(hr))
        return hr;

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear);
    if (FAILED(hr))
        return hr;

    ID3DBlob* skyboxVsBlob = nullptr;
    ID3DBlob* skyboxPsBlob = nullptr;

    hr = D3DCompileFromFile(
        L"skybox_vs.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_4_0",
        0,
        0,
        &skyboxVsBlob,
        nullptr
    );
    if (FAILED(hr))
        return hr;

    hr = D3DCompileFromFile(
        L"skybox_ps.hlsl",
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_4_0",
        0,
        0,
        &skyboxPsBlob,
        nullptr
    );
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateVertexShader(
        skyboxVsBlob->GetBufferPointer(),
        skyboxVsBlob->GetBufferSize(),
        nullptr,
        &g_pSkyboxVS
    );
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreatePixelShader(
        skyboxPsBlob->GetBufferPointer(),
        skyboxPsBlob->GetBufferSize(),
        nullptr,
        &g_pSkyboxPS
    );
    if (FAILED(hr))
        return hr;

    D3D11_INPUT_ELEMENT_DESC skyboxLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = g_pd3dDevice->CreateInputLayout(
        skyboxLayout,
        1,
        skyboxVsBlob->GetBufferPointer(),
        skyboxVsBlob->GetBufferSize(),
        &g_pSkyboxInputLayout
    );
    if (FAILED(hr))
        return hr;

    skyboxVsBlob->Release();
    skyboxPsBlob->Release();

    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(SkyboxVertex) * ARRAYSIZE(g_SkyboxVertices);
    ibd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    initData.pSysMem = g_SkyboxVertices;
    hr = g_pd3dDevice->CreateBuffer(&ibd, &initData, &g_pSkyboxVertexBuffer);
    if (FAILED(hr))
        return hr;

    vpBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vpBufferDesc.ByteWidth = sizeof(XMMATRIX);
    vpBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vpBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = g_pd3dDevice->CreateBuffer(&vpBufferDesc, nullptr, &g_pSkyboxVPBuffer);
    if (FAILED(hr))
        return hr;

    hr = CreateDDSTextureFromFile(g_pd3dDevice, L"skybox.dds", nullptr, &g_pSkyboxTextureRV);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Render() {
    m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    m_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, g_ClearColor);
    m_pDeviceContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    g_CubeAngle += 0.01f;
    if (g_CubeAngle > XM_2PI) g_CubeAngle -= XM_2PI;
    XMMATRIX model = XMMatrixRotationY(g_CubeAngle);

    float radius = 5.0f;
    float x = radius * sinf(g_CameraAzimuth) * cosf(g_CameraElevation);
    float y = radius * sinf(g_CameraElevation);
    float z = radius * cosf(g_CameraAzimuth) * cosf(g_CameraElevation);

    XMVECTOR eyePos = XMVectorSet(x, y, z, 0.0f);
    XMVECTOR focusPoint = XMVectorZero();
    XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(eyePos, focusPoint, upDir);

    RECT rc;
    GetClientRect(FindWindow(L"Laboratory work 4", L"Laboratory work 4"), &rc);
    float aspect = static_cast<float>(rc.right - rc.left) / (rc.bottom - rc.top);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 100.0f);

    XMMATRIX vp = view * proj;

    XMMATRIX mT = XMMatrixTranspose(model);
    XMMATRIX vpT = XMMatrixTranspose(vp);

    m_pDeviceContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &mT, 0, 0);

    XMMATRIX viewSkybox = view;
    viewSkybox.r[3] = XMVectorSet(0, 0, 0, 1);
    XMMATRIX vpSkybox = XMMatrixTranspose(viewSkybox * proj);

    D3D11_MAPPED_SUBRESOURCE mappedVP;
    HRESULT hr = m_pDeviceContext->Map(g_pSkyboxVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVP);
    if (SUCCEEDED(hr)) {
        memcpy(mappedVP.pData, &vpSkybox, sizeof(XMMATRIX));
        m_pDeviceContext->Unmap(g_pSkyboxVPBuffer, 0);
    }

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    ID3D11DepthStencilState *pDSStateSkybox = nullptr;
    g_pd3dDevice->CreateDepthStencilState(&dsDesc, &pDSStateSkybox);
    m_pDeviceContext->OMSetDepthStencilState(pDSStateSkybox, 0);

    D3D11_RASTERIZER_DESC rsDesc = {};
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.CullMode = D3D11_CULL_FRONT;
    rsDesc.FrontCounterClockwise = false;
    ID3D11RasterizerState* pSkyboxRS = nullptr;
    hr = g_pd3dDevice->CreateRasterizerState(&rsDesc, &pSkyboxRS);
    if (SUCCEEDED(hr)) {
        m_pDeviceContext->RSSetState(pSkyboxRS);
    }

    UINT stride = sizeof(SkyboxVertex);
    UINT offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &g_pSkyboxVertexBuffer, &stride, &offset);
    m_pDeviceContext->IASetInputLayout(g_pSkyboxInputLayout);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->VSSetShader(g_pSkyboxVS, nullptr, 0);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pSkyboxVPBuffer);
    m_pDeviceContext->PSSetShader(g_pSkyboxPS, nullptr, 0);
    m_pDeviceContext->PSSetShaderResources(0, 1, &g_pSkyboxTextureRV);
    m_pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
    m_pDeviceContext->Draw(ARRAYSIZE(g_SkyboxVertices), 0);

    pDSStateSkybox->Release();
    if (pSkyboxRS) {
        pSkyboxRS->Release();
        m_pDeviceContext->RSSetState(nullptr);
    }

    m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, g_pDepthStencilView);

    hr = m_pDeviceContext->Map(g_pVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVP);
    if (SUCCEEDED(hr)) {
        memcpy(mappedVP.pData, &vpT, sizeof(XMMATRIX));
        m_pDeviceContext->Unmap(g_pVPBuffer, 0);
    }

    stride = sizeof(Vertex);
    offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->IASetInputLayout(g_pVertexLayout);
    m_pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
    m_pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);
    m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pModelBuffer);
    m_pDeviceContext->VSSetConstantBuffers(1, 1, &g_pVPBuffer);
    m_pDeviceContext->PSSetShaderResources(0, 1, &g_pCubeTextureRV);
    m_pDeviceContext->PSSetSamplers(0, 1, &g_pSamplerLinear);
    m_pDeviceContext->Draw(ARRAYSIZE(g_CubeVertices), 0);

    g_pSwapChain->Present(1, 0);
}

void CleanupDevice() {
    if (m_pDeviceContext) m_pDeviceContext->ClearState();
    if (g_pVertexBuffer) g_pVertexBuffer->Release();
    if (g_pVertexLayout) g_pVertexLayout->Release();
    if (g_pVertexShader) g_pVertexShader->Release();
    if (g_pPixelShader) g_pPixelShader->Release();
    if (g_pModelBuffer) g_pModelBuffer->Release();
    if (g_pVPBuffer) g_pVPBuffer->Release();
    if (g_pCubeTextureRV) g_pCubeTextureRV->Release();
    if (g_pSamplerLinear) g_pSamplerLinear->Release();
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pDepthStencilView) g_pDepthStencilView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (m_pDeviceContext) m_pDeviceContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
    if (g_pSkyboxVertexBuffer) g_pSkyboxVertexBuffer->Release();
    if (g_pSkyboxInputLayout) g_pSkyboxInputLayout->Release();
    if (g_pSkyboxVS) g_pSkyboxVS->Release();
    if (g_pSkyboxPS) g_pSkyboxPS->Release();
    if (g_pSkyboxVPBuffer) g_pSkyboxVPBuffer->Release();
    if (g_pSkyboxTextureRV) g_pSkyboxTextureRV->Release();
}