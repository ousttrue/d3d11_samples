#pragma once

#include <banana/scene_command.h>
#include <d3d11.h>
#include <gorilla/mesh.h>
#include <gorilla/render_target.h>
#include <gorilla/window.h>
#include <memory>
#include <span>
#include <unordered_map>
#include <vector>
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

  void draw(const ComPtr<ID3D11Device> &device,
            const ComPtr<ID3D11DeviceContext> &context,
            const banana::DrawCommand &command);
};

class SceneRenderer {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  ResourceManager _resource_manager;

public:
  void Render(const ComPtr<ID3D11Device> &device,
              const ComPtr<ID3D11DeviceContext> &context,
              const DirectX::XMMATRIX &projection,
              const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &parent,
              const std::shared_ptr<banana::Node> &node);
};
