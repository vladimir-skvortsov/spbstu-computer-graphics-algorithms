#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Global variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* m_pDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
DirectX::XMFLOAT4 g_ClearColor = { 0.2f, 0.2f, 0.4f, 1.0f };

// Function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitDevice(HWND hWnd);
void CleanupDevice();
void Render();

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Register window
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        hInstance,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        L"Laboratory work 1",
        nullptr
    };
    RegisterClassEx(&wc);

    // Create window
    HWND hWnd = CreateWindow(
        wc.lpszClassName,
        L"Laboratory work 1",
        WS_OVERLAPPEDWINDOW,
        100,
        100,
        1280,
        720,
        nullptr,
        nullptr,
        wc.hInstance,
        nullptr
    );

    // Init DirectX
    if (FAILED(InitDevice(hWnd))) {
        CleanupDevice();
        return 0;
    }

    // Show window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main loop
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

    // Cleanup
    CleanupDevice();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            {
                HDC hdc = (HDC)wParam;
                RECT rect;
                GetClientRect(hWnd, &rect);

                COLORREF clearColor = RGB(
                    (BYTE)(g_ClearColor.x * 255),
                    (BYTE)(g_ClearColor.y * 255),
                    (BYTE)(g_ClearColor.z * 255)
                );

                HBRUSH brush = CreateSolidBrush(clearColor);
                FillRect(hdc, &rect, brush);
                DeleteObject(brush);

                return TRUE;
            }
            break;
        case WM_SIZE:
            if (g_pd3dDevice != nullptr) {
                if (g_pRenderTargetView) {
                    g_pRenderTargetView->Release();
                    g_pRenderTargetView = nullptr;
                }

                g_pSwapChain->ResizeBuffers(0, LOWORD(lParam), HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);

                // Recreate render target view
                ID3D11Texture2D* pBackBuffer = nullptr;
                g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
                g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
                pBackBuffer->Release();
            }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

HRESULT InitDevice(HWND hWnd) {
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT flags  = 0;

    #ifdef _DEBUG
        flags |= D3D11_CREATE_DEVICE_DEBUG;
    #endif

    D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

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
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_FEATURE_LEVEL featureLevel;

    hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        flags,
        featureLevels,
        numFeatureLevels,
        D3D11_SDK_VERSION,
        &sd,
        &g_pSwapChain,
        &g_pd3dDevice,
        &featureLevel,
        &m_pDeviceContext
    );

    assert(featureLevel == D3D_FEATURE_LEVEL_11_0);
    assert(SUCCEEDED(hr));

    if (FAILED(hr))
        return hr;

    // Create render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    m_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    D3D11_VIEWPORT viewport;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = (FLOAT)width;
    viewport.Height = (FLOAT)height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_pDeviceContext->RSSetViewports(1, &viewport);

    return S_OK;
}

void CleanupDevice() {
    if (m_pDeviceContext)
        m_pDeviceContext->ClearState();
    if (g_pRenderTargetView)
        g_pRenderTargetView->Release();
    if (g_pSwapChain)
        g_pSwapChain->Release();
    if (g_pd3dDevice)
        g_pd3dDevice->Release();
}

void Render() {
    // Clear background buffer
    m_pDeviceContext->ClearRenderTargetView(
        g_pRenderTargetView,
        reinterpret_cast<const float *>(&g_ClearColor)
    );

    // Show frame
    g_pSwapChain->Present(0, 0);
}