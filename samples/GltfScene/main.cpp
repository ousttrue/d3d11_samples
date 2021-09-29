#include "banana/orbit_camera.h"
#include "gorilla/window.h"
#include <app.h>
#include <banana/gltf.h>
#include <filesystem>
#include <imgui.h>
#include <iostream>
#include <renderer.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "GltfScene";

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class GltfSamples {
  std::shared_ptr<banana::Node> _root;
  std::string _base;

  struct Entry {
    std::string model;
    std::string dir;
    std::string file;

    std::string asset_key;

    Entry(const std::filesystem::path &path) {

      // name
      file = path.filename().generic_string();
      dir = path.parent_path().filename().generic_string();
      model = path.parent_path().parent_path().filename().generic_string();

      // key
      std::filesystem::path p;
      auto it = path.begin();
      ++it; // assets
      ++it; // gltF_Sample_Models
      ++it; // 2.0
      for (; it != path.end(); ++it) {
        p /= *it;
      }
      asset_key = p.generic_string();
    }
  };

  std::vector<Entry> _list;
  int selected = -1;

public:
  banana::OrbitCamera camera;

  GltfSamples(std::string_view base) : _base(base) {
    // std::filesystem::path root("assets/glTF-Sample-Models/2.0");
    std::filesystem::path assets("assets");
    for (auto const &e :
         std::filesystem::recursive_directory_iterator(assets / base)) {
      auto path = e.path();
      auto ext = path.extension();
      if (ext == ".glb" || ext == ".gltf") {
        _list.push_back(path);
      }
    }
  }

  bool gui() {
    if (ImGui::Begin("glTF-Sample-Models")) {
      int n = 0;
      if (ImGui::BeginTable("list", 3, ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("model", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("dir", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("file", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();
        int n = 0;
        for (auto &e : _list) {
          ImGui::TableNextRow();
          // 0
          ImGui::TableSetColumnIndex(0);
          ImGui::Text(e.model.c_str());
          // 1
          ImGui::TableSetColumnIndex(1);
          ImGui::Text(e.dir.c_str());
          // 2
          ImGui::TableSetColumnIndex(2);
          if (ImGui::Selectable(e.file.c_str(), n == selected,
                                ImGuiSelectableFlags_SpanAllColumns |
                                    ImGuiSelectableFlags_AllowItemOverlap)) {
            if (selected != n) {
              selected = n;
              load(e.asset_key);
            }
          }
          ++n;
        }
        ImGui::EndTable();
      }
    }
    ImGui::End();

    auto &io = ImGui::GetIO();
    return io.WantCaptureMouse;
  }

  std::shared_ptr<banana::Node> root() const { return _root; }

  // "glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb"
  // "glTF-Sample-Models/2.0/Avocado/glTF-Binary/Avocado.glb"
  // "glTF-Sample-Models/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"
  // "glTF-Sample-Models/2.0/CesiumMilkTruck/glTF-Binary/CesiumMilkTruck.glb"
  bool load(const std::string &sample) {
    auto key = _base + sample;
    banana::gltf::GltfLoader loader;
    if (!loader.load_from_asset(key)) {
      return {};
    }
    _root = loader.scenes[0].nodes[0];

    banana::AABB aabb;
    _root->calc_aabb(banana::Matrix4x4::identity(), &aabb);
    if (aabb.min.y < 0) {
      _root->transform.translation.y -= aabb.min.y;
    }
    auto half_height = aabb.height() / 2;
    camera.fit(half_height, half_height);

    return true;
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

  GltfSamples samples("glTF-Sample-Models/2.0/");
  if (!samples.load("CesiumMilkTruck/glTF-Binary/CesiumMilkTruck.glb")) {
    return 2;
  }

  // main loop
  Renderer renderer;
  auto context = app.context();
  gorilla::ScreenState state;
  while (app.begin_frame(&state)) {

    auto gui_focus = samples.gui();
    if (!gui_focus) {
      update_camera(&samples.camera, state);
    }

    renderer.render(device, context, samples.root(), &samples.camera);
    app.end_frame();
  }

  return 0;
}
