#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// Global variables
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pImmediateContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;
float CLEAR_COLOR[4] = { 0.2f, 0.2f, 0.4f, 1.0f };

// Function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT InitDevice(HWND hWnd);
void CleanupDevice();
void Render();

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Register window
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L,
        0L,
        hInstance,
        NULL,
        NULL,
        NULL,
        NULL,
        L"DirectX App",
        NULL
    };
    RegisterClassEx(&wc);

    // Create window
    HWND hWnd = CreateWindow(wc.lpszClassName, L"DirectX Application",
        WS_OVERLAPPEDWINDOW, 100, 100, 1280, 720,
        NULL, NULL, wc.hInstance, NULL);

    // Init DirectX
    if (FAILED(InitDevice(hWnd)))
    {
        CleanupDevice();
        return 0;
    }

    // Show window
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main loop
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            Render();
        }
    }

    // Cleanup
    CleanupDevice();
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return (int)msg.wParam;
}

// Handle messages
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr)
        {
            if (g_pRenderTargetView)
            {
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

HRESULT InitDevice(HWND hWnd)
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect(hWnd, &rc);
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
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

    D3D_FEATURE_LEVEL featureLevel;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        hr = D3D11CreateDeviceAndSwapChain(nullptr, driverTypes[driverTypeIndex], nullptr,
            createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &g_pSwapChain,
            &g_pd3dDevice, &featureLevel, &g_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
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

    g_pImmediateContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);

    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports(1, &vp);

    return S_OK;
}

// Cleanup device
void CleanupDevice()
{
    if (g_pImmediateContext) g_pImmediateContext->ClearState();

    if (g_pRenderTargetView) g_pRenderTargetView->Release();
    if (g_pSwapChain) g_pSwapChain->Release();
    if (g_pImmediateContext) g_pImmediateContext->Release();
    if (g_pd3dDevice) g_pd3dDevice->Release();
}

// Render frame
void Render()
{
    // Clear with color
    g_pImmediateContext->ClearRenderTargetView(g_pRenderTargetView, CLEAR_COLOR);

    // Show frame
    g_pSwapChain->Present(0, 0);
}