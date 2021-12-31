#pragma once
#include <functional>
#include <string>

namespace gorilla {

using DockFunc = std::function<void(bool *p_open)>;
struct Dock {
  std::string name;
  DockFunc show;
  bool open = true;
};
void dockspace(std::list<Dock> &docks);

} // namespace gorilla