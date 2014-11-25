#include "D3D11Manager.h"
#include "resource.h"

auto szTitle = L"MinTriangle";
auto szWindowClass = L"MinTriangle";
auto textureFile = L"../MinTriangle/texture.png";


static std::string GetShader(HINSTANCE hInst)
{
	HRSRC hRes = FindResource(hInst, MAKEINTRESOURCE(ID_SHADERSOURCE), L"SHADERSOURCE");
	HGLOBAL hMem = LoadResource(hInst, hRes);
	DWORD size = SizeofResource(hInst, hRes);
	char* resText = (char*)LockResource(hMem);
	char* text = (char*)malloc(size + 1);
	memcpy(text, resText, size);
	text[size] = 0;
	std::string s(text);
	free(text);
	FreeResource(hMem);
	return s;
}


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto d3d = (D3D11Manager*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (message)
    {
        case WM_CREATE:
            {
                auto d3d = (D3D11Manager*)((LPCREATESTRUCT)lParam)->lpCreateParams;
                SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)d3d);
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
        HINSTANCE hInstance,
        HINSTANCE hPrevInstance,
        LPSTR lpCmdLine,
        int nCmdShow
        )
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    D3D11Manager d3d11;

    // create window
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
    HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW
		, CW_USEDEFAULT, CW_USEDEFAULT
		, 320, 320
		, NULL, NULL, hInstance, &d3d11);
    if (!hWnd)
    {
        return 1;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    auto shaderSource=GetShader(hInstance);
    if(shaderSource.empty()){
        return 5;
    }

    // d3d
    if (!d3d11.Initialize(hWnd, shaderSource, textureFile)){
        return 2;
    }

    // main loop
    MSG msg;
    while (true)
    {
        if (PeekMessage (&msg,NULL,0,0,PM_NOREMOVE))
        {
            if (!GetMessage (&msg,NULL,0,0))
                return (int)msg.wParam ;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            d3d11.Render();
        }
    }
    return (int) msg.wParam;
}
