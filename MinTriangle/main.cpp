#include "CompileShaderFromFile.h"
#include <d3d11.h>
#include <DirectXMath.h>

auto szTitle = L"MinTriangle";
auto szWindowClass = L"MinTriangle";
auto szShaderFile = L"../MinTriangle/MinTriangle.fx";


struct Vertex
{
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
};
const D3D11_INPUT_ELEMENT_DESC vbElement[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
};


class D3D11Manager
{
	IDXGISwapChain*         pSwapChain;
	ID3D11Device*           pDevice;
	ID3D11DeviceContext*    pDeviceContext;

	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11Texture2D*        pDepthStencil;
	ID3D11DepthStencilView* pDepthStencilView;

	ID3D11Buffer* pVertexBuf;
	ID3D11Buffer* pIndexBuf;
	ID3D11VertexShader* pVsh;
	ID3D11PixelShader*  pPsh;
	ID3D11InputLayout* pInputLayout;
	ID3D11RasterizerState* pRS;
	ID3D11DepthStencilState* pDS;

	HWND g_hWnd = NULL;
	int g_width = 0;
	int g_height = 0;

public:
	D3D11Manager()
		: pSwapChain(NULL), pDevice(NULL), pDeviceContext(NULL)
		, pRenderTargetView(NULL), pDepthStencil(NULL), pDepthStencilView(NULL)
		, pVertexBuf(NULL), pIndexBuf(NULL)
		, pVsh(NULL), pPsh(NULL)
		, pInputLayout(NULL)
		, pRS(NULL), pDS(NULL)
	{

	}

	~D3D11Manager()
	{
#define SAFE_RELEASE(p)		if(p) { p->Release(); p = NULL; }

		SAFE_RELEASE(pDepthStencilView);
		SAFE_RELEASE(pDepthStencil);
		SAFE_RELEASE(pRenderTargetView);

		SAFE_RELEASE(pSwapChain);
		SAFE_RELEASE(pDevice);
		SAFE_RELEASE(pDeviceContext);

#undef SAFE_RELEASE
	}

	bool InitD3D11(HWND hWnd, const wchar_t *shaderFile)
	{
		g_hWnd = hWnd;
		RECT rect;
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		g_width = width;
		g_height = height;

		D3D_DRIVER_TYPE	dtype = D3D_DRIVER_TYPE_HARDWARE;
		UINT            flags = 0;
		D3D_FEATURE_LEVEL featureLevels[] = // featureLevel（新）
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};
		UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
		UINT sdkVersion = D3D11_SDK_VERSION;
		D3D_FEATURE_LEVEL validFeatureLevel;

		// スワップチェーンの設定(D3D9でいうD3DPRESENT_PARAMETERS相当の設定)
		DXGI_SWAP_CHAIN_DESC scDesc;
		ZeroMemory(&scDesc, sizeof(scDesc));
		scDesc.BufferCount = 1;
		scDesc.BufferDesc.Width = width;
		scDesc.BufferDesc.Height = height;
		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		scDesc.BufferDesc.RefreshRate.Numerator = 60;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.OutputWindow = hWnd;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.Windowed = TRUE;

		// デバイスとデバイスコンテキストとスワップチェーンの作成
		IDXGIAdapter* adapter = NULL;
		HRESULT	hr = D3D11CreateDeviceAndSwapChain(
			adapter,
			dtype,
			NULL,
			flags,
			featureLevels,
			numFeatureLevels,
			sdkVersion,
			&scDesc,
			&pSwapChain,
			&pDevice,
			&validFeatureLevel,
			&pDeviceContext);
		if (FAILED(hr))
			return false;

		if (setDefaultBackBufferAndDepthBuffer(width, height) == false)
			return false;

		if (init(shaderFile) == false)
			return false;

		return true;
	}

	void Resize(int w, int h)
	{
		g_width = w;
		g_height = h;
	}

	void Render()
	{
		// クリア
		Clear();

		// VBのセット
		ID3D11Buffer* pBufferTbl[] = { pVertexBuf };
		UINT SizeTbl[] = { sizeof(Vertex) };
		UINT OffsetTbl[] = { 0 };
		pDeviceContext->IASetVertexBuffers(0, 1, pBufferTbl, SizeTbl, OffsetTbl);
		// IBのセット
		pDeviceContext->IASetIndexBuffer(pIndexBuf, DXGI_FORMAT_R32_UINT, 0);

		// ILのセット
		pDeviceContext->IASetInputLayout(pInputLayout);
		// プリミティブタイプのセット
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		// Shaderのセットアップ
		pDeviceContext->VSSetShader(pVsh, NULL, 0);
		pDeviceContext->PSSetShader(pPsh, NULL, 0);

		// Stateの設定
		pDeviceContext->RSSetState(pRS);
		pDeviceContext->OMSetDepthStencilState(pDS, 0);

		// 描画
		pDeviceContext->DrawIndexed(3, 0, 0);
		pDeviceContext->Flush();

		// バッファスワップ
		SwapBuffer();
	}

private:
	bool createVB()
	{
		// Create VB
		Vertex pVertices[] =
		{
			{ DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT4(0.5f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
		};
		unsigned int vsize = sizeof(pVertices);

		D3D11_BUFFER_DESC vdesc;
		ZeroMemory(&vdesc, sizeof(vdesc));
		vdesc.ByteWidth = vsize;
		vdesc.Usage = D3D11_USAGE_DEFAULT;
		vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vdesc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA vertexData;
		ZeroMemory(&vertexData, sizeof(vertexData));
		vertexData.pSysMem = pVertices;

		HRESULT hr = pDevice->CreateBuffer(&vdesc, &vertexData, &pVertexBuf);
		if (FAILED(hr))
			return false;

		return true;
	}

	bool createIB()
	{
		HRESULT hr;

		// Create IB
		unsigned int pIndices[] =
		{
			0, 1, 2
		};
		unsigned int isize = sizeof(pIndices);

		D3D11_BUFFER_DESC idesc;
		ZeroMemory(&idesc, sizeof(idesc));
		idesc.ByteWidth = isize;
		idesc.Usage = D3D11_USAGE_DYNAMIC;
		idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		idesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		hr = pDevice->CreateBuffer(&idesc, NULL, &pIndexBuf);
		if (FAILED(hr))
			return false;

		D3D11_MAPPED_SUBRESOURCE ires;
		hr = pDeviceContext->Map(pIndexBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &ires);
		if (FAILED(hr))
			return false;

		void* idxbuf = ires.pData;
		if (idxbuf)
		{
			memcpy(idxbuf, pIndices, isize);
			pDeviceContext->Unmap(pIndexBuf, 0);
		}

		return true;
	}

	bool createShaders(const wchar_t *shaderFile)
	{
		HRESULT hr;
		// vertex shader
		ID3DBlob* vblob = NULL;
		hr = CompileShaderFromFile(shaderFile, "vsMain", "vs_4_0_level_9_1", &vblob);
		if (FAILED(hr))
			return false;
		hr = pDevice->CreateVertexShader(vblob->GetBufferPointer(), vblob->GetBufferSize(), NULL, &pVsh);
		if (FAILED(hr))
			return false;

		// pixel shader
		ID3DBlob* pblob = NULL;
		hr = CompileShaderFromFile(shaderFile, "psMain", "ps_4_0_level_9_1", &pblob);
		if (FAILED(hr))
			return false;
		hr = pDevice->CreatePixelShader(pblob->GetBufferPointer(), pblob->GetBufferSize(), NULL, &pPsh);
		if (FAILED(hr))
			return false;

		// Create InputLayout
		hr = pDevice->CreateInputLayout(vbElement, sizeof(vbElement) / sizeof(D3D11_INPUT_ELEMENT_DESC),
			vblob->GetBufferPointer(), vblob->GetBufferSize(), &pInputLayout);
		if (FAILED(hr))
			return false;

		return true;
	}

	bool init(const wchar_t *shaderFile)
	{
		if (createVB() == false)
			return false;

		if (createIB() == false)
			return false;

		if (createShaders(shaderFile) == false)
			return false;

		// render state
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
		rsDesc.CullMode = D3D11_CULL_BACK;
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.DepthClipEnable = TRUE;
		if (FAILED(pDevice->CreateRasterizerState(&rsDesc, &pRS)))
			return false;

		// depth stencil state
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		dsDesc.StencilEnable = FALSE;
		if (FAILED(pDevice->CreateDepthStencilState(&dsDesc, &pDS)))
			return false;

		return true;
	}

	bool setDefaultBackBufferAndDepthBuffer(int w, int h)
	{
		HRESULT hr = S_OK;

		// バックバッファの取得
		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			reinterpret_cast<void**>(&pBackBuffer));

		// RenderTargetViewの作成
		hr = pDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr))
			return false;

		// デプステクスチャの作成
		D3D11_TEXTURE2D_DESC depthDesc;
		ZeroMemory(&depthDesc, sizeof(depthDesc));
		depthDesc.Width = w;
		depthDesc.Height = h;
		depthDesc.MipLevels = 1;
		depthDesc.ArraySize = 1;
		depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthDesc.SampleDesc.Count = 1;
		depthDesc.SampleDesc.Quality = 0;
		depthDesc.Usage = D3D11_USAGE_DEFAULT;
		depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthDesc.CPUAccessFlags = 0;
		depthDesc.MiscFlags = 0;
		hr = pDevice->CreateTexture2D(&depthDesc, NULL, &pDepthStencil);
		if (FAILED(hr))
			return false;

		// DepthStencilViewの作成
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		ZeroMemory(&dsvDesc, sizeof(dsvDesc));
		dsvDesc.Format = depthDesc.Format;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Texture2D.MipSlice = 0;
		hr = pDevice->CreateDepthStencilView(pDepthStencil, &dsvDesc, &pDepthStencilView);
		if (FAILED(hr))
			return false;

		// レンダリングターゲットを設定する
		pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

		// ついでにビューポートの設定
		D3D11_VIEWPORT	vp;
		vp.Width = static_cast<float>(w);
		vp.Height = static_cast<float>(h);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		pDeviceContext->RSSetViewports(1, &vp);

		return true;
	}

	void Clear()
	{
		float clearColor[] = { 0.0f, 0.0f, 1.0f, 0.0f };
		pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);
		pDeviceContext->ClearDepthStencilView(pDepthStencilView,
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void SwapBuffer()
	{
		pSwapChain->Present(NULL, NULL);
	}
};


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto d3d = (D3D11Manager*)GetWindowLongPtr(hWnd, GWL_USERDATA);

	switch (message)
	{
	case WM_CREATE:
	{
		auto d3d = (D3D11Manager*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)d3d);
		break;
	}

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		d3d->Resize(LOWORD(wParam), HIWORD(wParam));
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}


int WINAPI WinMain(
	HINSTANCE hInstance,      // 現在のインスタンスのハンドル
	HINSTANCE hPrevInstance,  // 以前のインスタンスのハンドル
	LPSTR lpCmdLine,          // コマンドライン
	int nCmdShow              // 表示状態
	)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	{
		WNDCLASSEX wcex;
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hIcon = NULL;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;//MAKEINTRESOURCE(IDC_D3D11SAMPLE);
		wcex.lpszClassName = szWindowClass;
		wcex.hIconSm = NULL;
		RegisterClassEx(&wcex);
	}

	D3D11Manager d3d11;

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, &d3d11);
	if (!hWnd)
	{
		return 1;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	if (!d3d11.InitD3D11(hWnd, szShaderFile)){
		return 2;
	}

	// メイン メッセージ ループ:
	MSG msg;
	while (true)
	{
		if (PeekMessage (&msg,NULL,0,0,PM_NOREMOVE))
		{
			if (!GetMessage (&msg,NULL,0,0))
				return msg.wParam ;

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			d3d11.Render();
		}
	}

	return (int) msg.wParam;
}
