#pragma once
#include <banana/scene.h>
#include <d3d11.h>
#include <gorilla/constant_buffer.h>
#include <gorilla/input_assembler.h>
#include <gorilla/pipeline.h>
#include <gorilla/texture.h>
#include <memory>
#include <unordered_map>
#include <wrl/client.h>

namespace gorilla::resource {

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Material {
  Pipeline pipeline;
  std::shared_ptr<gorilla::Texture> color_texture;
  ComPtr<ID3D11RasterizerState> rs;
};

struct SubMesh {
  UINT offset = 0;
  UINT draw_count = 0;
  std::shared_ptr<Material> material;
};

struct Mesh {
  InputAssembler ia;
  std::vector<SubMesh> submeshes;
  void draw(const ComPtr<ID3D11DeviceContext> &context,
            const DirectX::XMMATRIX &projection, const DirectX::XMMATRIX &view,
            const DirectX::XMMATRIX &model);
};

class ResourceManager {
  std::unordered_map<std::shared_ptr<banana::Image>, std::shared_ptr<Texture>>
      _texture_map;
  std::unordered_map<std::shared_ptr<banana::Material>,
                     std::shared_ptr<Material>>
      _material_map;
  std::unordered_map<std::shared_ptr<banana::Mesh>, std::shared_ptr<Mesh>>
      _mesh_map;

public:
  std::shared_ptr<Texture>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Image> &src);
  std::shared_ptr<Material>
  get_or_create(const ComPtr<ID3D11Device> &device,
                const std::shared_ptr<banana::Material> &src);
  std::shared_ptr<Mesh> get_or_create(const ComPtr<ID3D11Device> &device,
                                      const std::shared_ptr<banana::Mesh> &src);
};

} // namespace gorilla::resource
