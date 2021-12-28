#include "dockspace.h"
#include <imgui.h>
#include <list>

auto DOCKSPACE = "__dockspace__";

void dockspace(std::list<Dock> &docks) {

  auto flags =
      (ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
       ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar |
       ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
       ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
       ImGuiWindowFlags_NoNavFocus);

  auto viewport = ImGui::GetMainViewport();
  auto pos = viewport->Pos;
  auto size = viewport->Size;
  ImGui::SetNextWindowPos(pos);
  ImGui::SetNextWindowSize(size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});

  // DockSpace
  ImGui::Begin(DOCKSPACE, nullptr, flags);
  ImGui::PopStyleVar(3);
  auto dockspace_id = ImGui::GetID(DOCKSPACE);
  ImGui::DockSpace(dockspace_id, {0, 0},
                   ImGuiDockNodeFlags_PassthruCentralNode);
  ImGui::End();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Views")) {
      for (auto &dock : docks) {
        ImGui::MenuItem(dock.name.c_str(), "", &dock.open);
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  for (auto &dock : docks) {
    if (dock.open) {
      dock.show(&dock.open);
    }
  }
}
