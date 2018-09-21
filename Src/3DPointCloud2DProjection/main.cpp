#include "stdafx.h"

using std::string;
using std::wstring;
#define SAFE_RELEASE(p) {if (p) { p->Release(); p=NULL;} }

enum State {
	RUN, STOP
};
State g_state = RUN;
ID3D11Device *g_d3dDevice; // thread mode (deferred mode) is reference
ID3D11DeviceContext *g_devContext;
IDXGISwapChain *g_swapChain;
ID3D11RenderTargetView *g_renderTargetView; // backbuffer
ID3D11DepthStencilView *g_depthStencilView;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void Render();

HWND InitWindow(HINSTANCE hInstance
	, const wstring &windowName
	, const RECT &windowRect
	, int nCmdShow
	, WNDPROC WndProc
	, const bool dualMonitor //=false
)
{
	const int X = (int)windowRect.left;
	const int Y = (int)windowRect.top;
	const int WIDTH = int(windowRect.right - windowRect.left);
	const int HEIGHT = int(windowRect.bottom - windowRect.top);

	WNDCLASS WndClass;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = windowName.c_str();
	WndClass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClass(&WndClass);

	HWND hWnd = CreateWindow(
		windowName.c_str(),
		windowName.c_str(),
		WS_OVERLAPPEDWINDOW,
		X,
		Y,
		WIDTH,
		HEIGHT,
		GetDesktopWindow(),
		NULL,
		hInstance,
		NULL
	);

	//윈도우를 정확한 작업영역 크기로 맞춘다
	RECT rcClient = { 0, 0, WIDTH, HEIGHT };
	AdjustWindowRect(&rcClient, WS_OVERLAPPEDWINDOW, FALSE);	//rcClient 크기를 작업 영영으로 할 윈도우 크기를 rcClient 에 대입되어 나온다.

	const int width = rcClient.right - rcClient.left;
	const int height = rcClient.bottom - rcClient.top;
	const int screenCX = GetSystemMetrics(SM_CXSCREEN) * (dualMonitor ? 2 : 1);
	const int screenCY = GetSystemMetrics(SM_CYSCREEN);
	const int x = screenCX / 2 - width / 2;
	int y = screenCY / 2 - height / 2;

	const float hRate = (float)height / (float)screenCY;
	if (hRate > 0.95f)
		y = 0;

	//윈도우 크기와 윈도우 위치를 바꾸어준다.
	SetWindowPos(hWnd, NULL, 0, 0, width, height, SWP_NOZORDER | SWP_NOMOVE);
	MoveWindow(hWnd, x, y, width, height, TRUE);

	ShowWindow(hWnd, nCmdShow);

	return hWnd;
}


bool InitDirectX11(const HWND hWnd, const float width, const float height
	, OUT ID3D11Device **pd3dDevice
	, OUT ID3D11DeviceContext **pImmediateContext
	, OUT IDXGISwapChain **pSwapChain
	, OUT ID3D11RenderTargetView **pRenderTargetView
	, OUT ID3D11DepthStencilView **pDepthStencilView
)
{
	HRESULT hr = S_OK;

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
	sd.BufferDesc.Width = (UINT)width;
	sd.BufferDesc.Height = (UINT)height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	//sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	//sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, pSwapChain, pd3dDevice, &featureLevel, pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer = NULL;
	hr = (*pSwapChain)->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = (*pd3dDevice)->CreateRenderTargetView(pBackBuffer, NULL, pRenderTargetView);
	pBackBuffer->Release();
	if (FAILED(hr))
		return false;

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth;
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = (UINT)width;
	descDepth.Height = (UINT)height;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ID3D11Texture2D *pDepthStencil = NULL;
	hr = (*pd3dDevice)->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	if (FAILED(hr))
		return false;

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = (*pd3dDevice)->CreateDepthStencilView(pDepthStencil, &descDSV, pDepthStencilView);
	if (FAILED(hr))
		return false;
	pDepthStencil->Release();

	(*pImmediateContext)->OMSetRenderTargets(1, pRenderTargetView, *pDepthStencilView);

	return true;
}


void Clear()
{
	SAFE_RELEASE(g_renderTargetView);
	SAFE_RELEASE(g_depthStencilView);
	SAFE_RELEASE(g_swapChain);
	SAFE_RELEASE(g_d3dDevice);
	SAFE_RELEASE(g_devContext);
}


int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	const int width = 800;
	const int height = 600;
	const HWND hWnd = InitWindow(hInstance,
		L"3DPointCloud - 2DProjection",
		{ 0,0,width,height},
		SW_SHOW,
		WndProc,
		false);

	if (hWnd == NULL)
		return 0;

	if (!InitDirectX11(hWnd, (float)width, (float)height, &g_d3dDevice, &g_devContext
		, &g_swapChain, &g_renderTargetView, &g_depthStencilView))
	{
		return 0;
	}

	MSG msg;
	while (RUN == g_state)
	{
		if (PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Render();
	}

	Clear();
	return 1;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		case VK_RETURN:
			break;
		}
		break;

	case WM_DESTROY:
		g_state = STOP;
		PostQuitMessage(0);
		break;

	case WM_ERASEBKGND:
		return 0;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void Render()
{



	g_swapChain->Present(0, 0);
}
