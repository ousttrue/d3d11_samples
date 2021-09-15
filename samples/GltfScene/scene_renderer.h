#pragma once
#include <DirectXMath.h>
#include <banana/scene.h>
#include <d3d11.h>
#include <memory>
#include <wrl/client.h>

class SceneRenderer {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  struct SceneRendererImpl *_impl = nullptr;

public:
  SceneRenderer();
  ~SceneRenderer();
  void Render(const ComPtr<ID3D11Device> &device,
              const ComPtr<ID3D11DeviceContext> &context,
              const DirectX::XMMATRIX &projection,
              const DirectX::XMMATRIX &view, const DirectX::XMMATRIX &parent,
              const std::shared_ptr<banana::Node> &node);
};
