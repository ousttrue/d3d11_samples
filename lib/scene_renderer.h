#pragma once

#include <banana/node.h>
#include <banana/orbit_camera.h>
#include <d3d11.h>
#include <gorilla/mesh.h>
#include <gorilla/render_target.h>
#include <gorilla/window.h>
#include <unordered_map>
#include <wrl/client.h>

class ResourceManager {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  std::unordered_map<std::shared_ptr<banana::Image>,
                     std::shared_ptr<gorilla::Texture>>
      _texture_map;
  std::unordered_map<std::shared_ptr<banana::Material>,
                     std::shared_ptr<gorilla::Material>>
      _material_map;
  std::unordered_map<std::shared_ptr<banana::Mesh>,
                     std::shared_ptr<gorilla::Mesh>>
      _mesh_map;

public:
  std::shared_ptr<gorilla::Texture>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Image> &src);
  std::shared_ptr<gorilla::Material>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Material> &src);
  std::shared_ptr<gorilla::Mesh>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Mesh> &src);
};

class SceneRenderer {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ResourceManager _resource_manager;

public:
  void Render(const ComPtr<ID3D11Device> &device,
              const ComPtr<ID3D11DeviceContext> &context,
              const DirectX::XMMATRIX &projection,
              const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &parent,
              const std::shared_ptr<banana::Node> &node) {

    auto local = node->transform.matrix();
    auto M = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4 *)&local), parent);

    if (node->mesh) {
      auto drawable = _resource_manager.get_or_create(device, node->mesh);
      drawable->draw(context, projection, view, M);
    }

    for (auto &child : node->children) {
      Render(device, context, projection, view, M, child);
    }
  }
};
