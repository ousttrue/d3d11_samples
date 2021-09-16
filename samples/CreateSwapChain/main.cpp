#include <gorilla/swapchain.h>
#include <gorilla/device.h>
#include <gorilla/window.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "CreateSwapChain";
auto WIDTH = 320;
auto HEIGHT = 320;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  gorilla::Window window;
  auto hwnd = window.create(hInstance, CLASS_NAME, WINDOW_TITLE, WIDTH, HEIGHT);
  if (!hwnd) {
    return 1;
  }

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  auto device = gorilla::create_device();
  if (!device) {
    return 2;
  }

  auto swapchain = gorilla::create_swapchain(device, hwnd);
  if (!swapchain) {
    return 3;
  }
  DXGI_SWAP_CHAIN_DESC desc;
  swapchain->GetDesc(&desc);

  for (UINT frame_count = 0; window.process_messages(); ++frame_count) {
    RECT rect;
    GetClientRect(hwnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    if (w != desc.BufferDesc.Width || h != desc.BufferDesc.Height) {
      swapchain->ResizeBuffers(desc.BufferCount, w, h, desc.BufferDesc.Format,
                               desc.Flags);
    }
    swapchain->Present(0, 0);
  }

  return 0;
}
