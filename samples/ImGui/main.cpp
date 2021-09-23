#include "banana/orbit_camera.h"
#include "gorilla/window.h"
#include <app.h>
#include <banana/gltf.h>
#include <chrono>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <renderer.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "ImGui";

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class Gui {
  bool show_demo_window = true;
  bool show_another_window = false;
  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  std::chrono::system_clock::time_point last = {};

public:
  Gui(const ComPtr<ID3D11Device> &device,
      const ComPtr<ID3D11DeviceContext> &context) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // io.ConfigFlags |=
    //     ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable
    // Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;   // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport
                                                        // / Platform Windows
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;
    // io.ConfigViewportsNoDefaultParent = true;
    // io.ConfigDockingAlwaysTabBar = true;
    // io.ConfigDockingTransparentPayload = true;
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI:
    // Experimental. THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER
    // APP! io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; //
    // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform
    // windows can look identical to regular ones.
    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    // ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), context.Get());
  }

  ~Gui() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    // ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
  }

  bool draw(const ComPtr<ID3D11DeviceContext> &context,
            const gorilla::ScreenState &state) {
    ImGuiIO &io = ImGui::GetIO();

    //
    // update custom backend
    //
    if (last == std::chrono::system_clock::time_point{}) {

    } else {
      io.DeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                         state.time - last)
                         .count() *
                     0.001f;
    }
    if (io.DeltaTime == 0) {
      io.DeltaTime = 0.016f;
    }
    last = state.time;
    io.DisplaySize = {state.width, state.height};
    io.MousePos = {state.mouse_x, state.mouse_y};
    io.MouseDown[0] = state.mouse_button_flag & gorilla::MouseButtonLeftDown;
    io.MouseDown[1] = state.mouse_button_flag & gorilla::MouseButtonRightDown;
    io.MouseWheel = state.wheel;

    // update

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    // ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 1. Show the big demo window (Most of the sample code is in
    // ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear
    // ImGui!).
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair
    // to created a named window.
    {
      static float f = 0.0f;
      static int counter = 0;

      ImGui::Begin("Hello, world!"); // Create a window called "Hello, world!"
                                     // and append into it.

      ImGui::Text("This is some useful text."); // Display some text (you can
                                                // use a format strings too)
      ImGui::Checkbox(
          "Demo Window",
          &show_demo_window); // Edit bools storing our window open/close state
      ImGui::Checkbox("Another Window", &show_another_window);

      ImGui::SliderFloat("float", &f, 0.0f,
                         1.0f); // Edit 1 float using a slider from 0.0f to 1.0f
      ImGui::ColorEdit3(
          "clear color",
          (float *)&clear_color); // Edit 3 floats representing a color

      if (ImGui::Button("Button")) // Buttons return true when clicked (most
                                   // widgets return true when edited/activated)
        counter++;
      ImGui::SameLine();
      ImGui::Text("counter = %d", counter);

      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window) {
      ImGui::Begin(
          "Another Window",
          &show_another_window); // Pass a pointer to our bool variable (the
                                 // window will have a closing button that will
                                 // clear the bool when clicked)
      ImGui::Text("Hello from another window!");
      if (ImGui::Button("Close Me"))
        show_another_window = false;

      ImGui::End();
    }

    // Rendering
    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return io.WantCaptureMouse;
  }
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  App app;
  auto device =
      app.initialize(hInstance, lpCmdLine, nCmdShow, CLASS_NAME, WINDOW_TITLE);
  if (!device) {
    return 1;
  }

  banana::gltf::GltfLoader loader;
  if (!loader.load_from_asset(
          // "glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb"
          // "glTF-Sample-Models/2.0/Avocado/glTF-Binary/Avocado.glb"
          // "glTF-Sample-Models/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"
          "glTF-Sample-Models/2.0/CesiumMilkTruck/glTF-Binary/"
          "CesiumMilkTruck.glb")) {
    return 2;
  }
  auto root = loader.scenes[0].nodes[0];

  // main loop
  Renderer renderer;
  auto context = app.context();
  Gui gui(device, context);
  banana::OrbitCamera camera;
  gorilla::ScreenState state;
  while (app.begin_frame(&state)) {
    // updage gui
    camera.resize(state.width, state.height);

    // draw
    renderer.render(device, context, root, &camera);

    if (!gui.draw(context, state)) {
      update_camera(&camera, state);
    }

    app.end_frame();
  }

  return 0;
}
