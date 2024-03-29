#include "gorilla/window.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PWSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  gorilla::Window window;
  auto hwnd = window.create(hInstance, "CLASS_NAME", "CreateWindow");
  if (!hwnd) {
    return 1;
  }
  window.load_ini();

  ShowWindow(hwnd, nCmdShow);
  UpdateWindow(hwnd);

  // main loop
  for (UINT frameCount = 0; window.process_messages(); ++frameCount) {
  }

  window.save_ini();

  return 0;
}
