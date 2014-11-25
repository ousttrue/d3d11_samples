#include <Windows.h>
#include "../01.CreateDevice/windowutil.h"
#include "../02.D2D_HUD/D2DManager.h"
#include "MinShaderPipeline.h"
#include <sstream>
#include <mmsystem.h>


auto title = L"MinTriangle";
auto windowClass = L"MinTriangle";
auto shader= L"../03.MinShaderPipeline/MinShaderPipeline.fx";
auto uifile="../02.D2D_HUD/D2D_HUD.ui.xml";

int WINAPI WinMain(
        HINSTANCE hInstance, // 現在のインスタンスのハンドル
        HINSTANCE hPrevInstance, // 以前のインスタンスのハンドル
        LPSTR lpCmdLine, // コマンドライン
        int nCmdShow // 表示状態
        )
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    DXGIManager dxgi;
    // d3d
    ShaderInfo info;
    info.path=shader;
    info.vs.entrypoint="vsMain";
    info.vs.model="vs_4_0_level_9_1";
    info.ps.entrypoint="psMain";
    info.ps.model="ps_4_0_level_9_1";
        
    auto d3d=std::make_shared<MinShaderPipeline>(info);
    dxgi.AddResourceManager(d3d);
    // d2d
    auto d2d=std::make_shared<D2DManager>();
    d2d->GetHUD()->Load(uifile);
    dxgi.AddResourceManager(d2d);

    auto hWnd=windowutil::NewWindow(windowClass, title, &dxgi);
    if(!hWnd)return 1;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // create dxgi
	if (!dxgi.CreateDevice())return 2;

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
            dxgi.Render(hWnd);
			auto now = timeGetTime();
			d2d->GetHUD()->Update(std::chrono::milliseconds(now));
        }
    }
    return (int) msg.wParam;
}
