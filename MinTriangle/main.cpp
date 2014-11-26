#include "D3D11Manager.h"

auto szTitle = L"MinTriangle";
auto szWindowClass = L"MinTriangle";
auto szShaderFile = L"../MinTriangle/MinTriangle.fx";


static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        HINSTANCE hInstance, // 現在のインスタンスのハンドル
        HINSTANCE hPrevInstance, // 以前のインスタンスのハンドル
        LPSTR lpCmdLine, // コマンドライン
        int nCmdShow // 表示状態
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

    // d3d
    if (!d3d11.Initialize(hWnd, szShaderFile)){
        return 2;
    }

    // main loop
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
        else {
            d3d11.Render();
        }
    }
    return (int) msg.wParam;
}

