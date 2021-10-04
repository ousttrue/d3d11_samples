#include <app.h>
#include <banana/gltf.h>
#include <banana/orbit_camera.h>
#include <filesystem>
#include <gorilla/gizmo.h>
#include <gorilla/window.h>
#include <imgui.h>
#include <iostream>
#include <renderer.h>

auto CLASS_NAME = "CLASS_NAME";
auto WINDOW_TITLE = "GltfScene";

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

class GltfSamples {
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
  std::shared_ptr<banana::Node> root;
  banana::AABB aabb;

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
        ImGui::TableSetupColumn("dir", ImGuiTableColumnFlags_WidthFixed,
                                100.0f);
        ImGui::TableSetupColumn("file", ImGuiTableColumnFlags_WidthFixed,
                                170.0f);
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

  // "glTF-Sample-Models/2.0/BoxTextured/glTF-Binary/BoxTextured.glb"
  // "glTF-Sample-Models/2.0/Avocado/glTF-Binary/Avocado.glb"
  // "glTF-Sample-Models/2.0/DamagedHelmet/glTF-Binary/DamagedHelmet.glb"
  // "glTF-Sample-Models/2.0/CesiumMilkTruck/glTF-Binary/CesiumMilkTruck.glb"
  bool load(const std::string &sample) {
    auto key = _base + sample;
    banana::gltf::GltfLoader loader;
    try {
      if (!loader.load_from_asset(key)) {
        return {};
      }
      root = loader.root;

      aabb = {};
      root->calc_aabb(banana::Matrix4x4::identity(), &aabb);
      banana::Float3 move{0, aabb.min.y, 0};
      root->transform.translation -= move;
      aabb.min += move;
      aabb.max += move;

      std::cerr << aabb.min.y << ", " << aabb.max.y << std::endl;
      camera.fit(aabb);
      std::cerr << camera.translation.y << std::endl;

      return true;
    } catch (const std::runtime_error &ex) {
      std::cerr << ex.what() << std::endl;
      return false;
    }
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

  auto grid = gorilla::gizmo::create_grid(device);

  GltfSamples samples("glTF-Sample-Models/2.0/");
  if (lpCmdLine) {
    samples.load(lpCmdLine);
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

    renderer.render(device, context, samples.root, &samples.camera);
    grid->draw(context, samples.camera);

    app.end_frame();
  }

  return 0;
}
