#include "windowutil.h"
#include "D3DManager.h"

namespace windowutil {

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_CREATE:
            {
                auto pD3D=(DXGIManager*)((LPCREATESTRUCT)lParam)->lpCreateParams;
                // set this pointer
                SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)pD3D);
            }
            return 0;

        case WM_ERASEBKGND:
            return 0;

        case WM_SIZE:
            if(auto pD3D=(DXGIManager*)GetWindowLongPtr(hWnd, GWL_USERDATA)){
                pD3D->Resize(LOWORD(lParam), HIWORD(lParam));
            }
            return 0;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC hdc = BeginPaint(hWnd, &ps);
                EndPaint(hWnd, &ps);
            }
            return 0;

        case WM_DESTROY:
			if (auto pD3D = (DXGIManager*)GetWindowLongPtr(hWnd, GWL_USERDATA)){
                pD3D->DestroyDevice();
            }
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}


HWND NewWindow(const std::wstring &windowClass
        , const std::wstring &winidowTitle
        , void *userdata)
{
    auto hInstance=GetModuleHandle(NULL);

    // register window class
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
    wcex.lpszClassName = windowClass.c_str();
    wcex.hIconSm = NULL;
    if(RegisterClassEx(&wcex)==0)return NULL;

    // create window
    return CreateWindow(windowClass.c_str(), winidowTitle.c_str()
            , WS_OVERLAPPEDWINDOW
            , CW_USEDEFAULT, CW_USEDEFAULT
            , CW_USEDEFAULT, CW_USEDEFAULT
            , NULL, NULL
            , hInstance, userdata);
}

}
