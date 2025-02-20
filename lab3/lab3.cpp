#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <chrono>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;

// Global variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* m_pDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
const float g_ClearColor[4] = {0.2f, 0.2f, 0.4f, 1.0f};

// Buffers and shaders
ID3D11VertexShader* g_pVertexShader = nullptr;
ID3D11PixelShader* g_pPixelShader = nullptr;
ID3D11InputLayout* g_pVertexLayout = nullptr;
ID3D11Buffer* g_pVertexBuffer = nullptr;

// Constant buffers
ID3D11Buffer* g_pModelBuffer = nullptr;
ID3D11Buffer* g_pVPBuffer = nullptr;

// Camera control
float g_CamYaw = 0.0f;
float g_CamPitch = 0.5f;

// Cube control
float g_CubeAngle = 0.0f;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitDevice(HWND hWnd);
void CleanupDevice();
void Render();
HRESULT CreateCubeResources();

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
};

Vertex g_CubeVertices[] = {
    { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(247.0f / 255.0f, 37.0f / 255.0f, 133.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT4(247.0f / 255.0f, 37.0f / 255.0f, 133.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, 0.5f), XMFLOAT4(247.0f / 255.0f, 37.0f / 255.0f, 133.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(247.0f / 255.0f, 37.0f / 255.0f, 133.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, 0.5f), XMFLOAT4(247.0f / 255.0f, 37.0f / 255.0f, 133.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(247.0f / 255.0f, 37.0f / 255.0f, 133.0f / 255.0f, 1) },

    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(114.0f / 255.0f, 9.0f / 255.0f, 183.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT4(114.0f / 255.0f, 9.0f / 255.0f, 183.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT4(114.0f / 255.0f, 9.0f / 255.0f, 183.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(114.0f / 255.0f, 9.0f / 255.0f, 183.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(114.0f / 255.0f, 9.0f / 255.0f, 183.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT4(114.0f / 255.0f, 9.0f / 255.0f, 183.0f / 255.0f, 1) },

    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(72.0f / 255.0f, 12.0f / 255.0f, 168.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(72.0f / 255.0f, 12.0f / 255.0f, 168.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(72.0f / 255.0f, 12.0f / 255.0f, 168.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(72.0f / 255.0f, 12.0f / 255.0f, 168.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(72.0f / 255.0f, 12.0f / 255.0f, 168.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(72.0f / 255.0f, 12.0f / 255.0f, 168.0f / 255.0f, 1) },

    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(58.0f / 255.0f, 12.0f / 255.0f, 163.0f / 255.0f, 1) },
    { XMFLOAT3(0.5f, 0.5f,  0.5f), XMFLOAT4(58.0f / 255.0f, 12.0f / 255.0f, 163.0f / 255.0f, 1) },
    { XMFLOAT3(0.5f, -0.5f, 0.5f), XMFLOAT4(58.0f / 255.0f, 12.0f / 255.0f, 163.0f / 255.0f, 1) },
    { XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT4(58.0f / 255.0f, 12.0f / 255.0f, 163.0f / 255.0f, 1) },
    { XMFLOAT3(0.5f, 0.5f, -0.5f), XMFLOAT4(58.0f / 255.0f, 12.0f / 255.0f, 163.0f / 255.0f, 1) },
    { XMFLOAT3(0.5f, 0.5f, 0.5f), XMFLOAT4(58.0f / 255.0f, 12.0f / 255.0f, 163.0f / 255.0f, 1) },

    { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(67.0f / 255.0f, 97.0f / 255.0f, 238.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, 0.5f), XMFLOAT4(67.0f / 255.0f, 97.0f / 255.0f, 238.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT4(67.0f / 255.0f, 97.0f / 255.0f, 238.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, 0.5f), XMFLOAT4(67.0f / 255.0f, 97.0f / 255.0f, 238.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, 0.5f, -0.5f), XMFLOAT4(67.0f / 255.0f, 97.0f / 255.0f, 238.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, 0.5f, -0.5f), XMFLOAT4(67.0f / 255.0f, 97.0f / 255.0f, 238.0f / 255.0f, 1) },

    { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(76.0f / 255.0f, 201.0f / 255.0f, 240.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT4(76.0f / 255.0f, 201.0f / 255.0f, 240.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, -0.5f, 0.5f), XMFLOAT4(76.0f / 255.0f, 201.0f / 255.0f, 240.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, -0.5f, 0.5f), XMFLOAT4(76.0f / 255.0f, 201.0f / 255.0f, 240.0f / 255.0f, 1) },
    { XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT4(76.0f / 255.0f, 201.0f / 255.0f, 240.0f / 255.0f, 1) },
    { XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT4(76.0f / 255.0f, 201.0f / 255.0f, 240.0f / 255.0f, 1) },
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hbrBackground = nullptr;
    wcex.lpszClassName = L"Laboratory work 3";

    RegisterClassExW(&wcex);

    HWND hWnd = CreateWindowW(
        L"Laboratory work 3",
        L"Laboratory work 3",
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
        }
        else {
            Render();
        }
    }

    CleanupDevice();
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_KEYDOWN:
        if (wParam == VK_LEFT) g_CamYaw += 0.1f;
        if (wParam == VK_RIGHT) g_CamYaw -= 0.1f;
        if (wParam == VK_UP) g_CamPitch += 0.1f;
        if (wParam == VK_DOWN) g_CamPitch -= 0.1f;
        break;
    case WM_SIZE:
        if (g_pd3dDevice && g_pSwapChain) {
            if (g_pRenderTargetView) {
                m_pDeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
                g_pRenderTargetView->Release();
                g_pRenderTargetView = nullptr;
            }

            g_pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

            ID3D11Texture2D* pBackBuffer = nullptr;
            g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
            g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
            pBackBuffer->Release();

            m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);
            RECT rc;
            GetClientRect(hWnd, &rc);
            D3D11_VIEWPORT vp = {};
            vp.Width = static_cast<FLOAT>(rc.right - rc.left);
            vp.Height = static_cast<FLOAT>(rc.bottom - rc.top);
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            m_pDeviceContext->RSSetViewports(1, &vp);
        }
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

    m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

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

    // Create constant buffers
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(XMMATRIX);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer(&cbDesc, nullptr, &g_pModelBuffer);
    if (FAILED(hr))
        return hr;

    D3D11_BUFFER_DESC vpDesc = {};
    vpDesc.Usage = D3D11_USAGE_DYNAMIC;
    vpDesc.ByteWidth = sizeof(XMMATRIX);
    vpDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vpDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    hr = g_pd3dDevice->CreateBuffer(&vpDesc, nullptr, &g_pVPBuffer);
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
        { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    hr = g_pd3dDevice->CreateInputLayout(
        layout,
        ARRAYSIZE(layout),
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
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = g_CubeVertices;

    hr = g_pd3dDevice->CreateBuffer(&ibd, &initData, &g_pVertexBuffer);
    if (FAILED(hr))
        return hr;

    return S_OK;
}

void Render() {
    m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    m_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, g_ClearColor);

    g_CubeAngle += 0.01f;
    if (g_CubeAngle > XM_2PI) g_CubeAngle -= XM_2PI;
    XMMATRIX model = XMMatrixRotationY(g_CubeAngle);

    float radius = 3.0f;
    float theta = g_CamYaw;
    float phi = g_CamPitch;

    float x = radius * cosf(g_CamYaw) * cosf(g_CamPitch);
    float y = radius * sinf(g_CamPitch);
    float z = radius * sinf(g_CamYaw) * cosf(g_CamPitch);

    XMVECTOR eyePos = XMVectorSet(x, y, z, 0.0f);
    XMVECTOR focusPoint = XMVectorZero();
    XMVECTOR upDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(eyePos, focusPoint, upDir);

    RECT rc;
    GetClientRect(FindWindow(L"Laboratory work 3", L"Laboratory work 3"), &rc);
    float aspect = static_cast<float>(rc.right - rc.left) / (rc.bottom - rc.top);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect, 0.01f, 100.0f);

    XMMATRIX vp = view * proj;

    XMMATRIX mT = XMMatrixTranspose(model);
    XMMATRIX vpT = XMMatrixTranspose(vp);

    m_pDeviceContext->UpdateSubresource(g_pModelBuffer, 0, nullptr, &mT, 0, 0);

    D3D11_MAPPED_SUBRESOURCE mappedVP;
    HRESULT hr = m_pDeviceContext->Map(g_pVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVP);
    if (SUCCEEDED(hr)) {
        memcpy(mappedVP.pData, &vpT, sizeof(XMMATRIX));
        m_pDeviceContext->Unmap(g_pVPBuffer, 0);
    }

    m_pDeviceContext->VSSetConstantBuffers(0, 1, &g_pModelBuffer);
    m_pDeviceContext->VSSetConstantBuffers(1, 1, &g_pVPBuffer);

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_pDeviceContext->IASetInputLayout(g_pVertexLayout);

    m_pDeviceContext->VSSetShader(g_pVertexShader, nullptr, 0);
    m_pDeviceContext->PSSetShader(g_pPixelShader, nullptr, 0);

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
    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (m_pDeviceContext) m_pDeviceContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}