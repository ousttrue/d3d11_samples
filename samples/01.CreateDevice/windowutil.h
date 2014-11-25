#include <windows.h>
#include <string>


namespace windowutil {

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

HWND NewWindow(const std::wstring &windowClass
        , const std::wstring &windowTitle
        , void *userdata);

}
