#pragma once
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

namespace gorilla {
class InputAssembler {
  template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  D3D11_BUFFER_DESC _vdesc = {0};
  ComPtr<ID3D11Buffer> _vb;
  UINT _vertex_count = 0;
  UINT _vertex_stride = 0;

  D3D11_BUFFER_DESC _idesc = {0};
  ComPtr<ID3D11Buffer> _ib;
  UINT _index_count = 0;
  UINT _index_stride = 0;
  DXGI_FORMAT _index_format = DXGI_FORMAT_R32_UINT;

public:
  // vertices
  bool create_dynamic_vertices(const ComPtr<ID3D11Device> &device,
                               size_t stride, size_t dynamic_size);

  bool create_vertices(const ComPtr<ID3D11Device> &device, size_t stride,
                       const void *p, size_t size);

  template <typename T>
  bool create_vertices(const ComPtr<ID3D11Device> &device, const T *p,
                       size_t count) {
    auto stride = sizeof(T);
    return create_vertices(device, stride, p, stride * count);
  }

  template <typename T>
  bool create_vertices(const ComPtr<ID3D11Device> &device,
                       const std::vector<T> &vertices) {
    return create_vertices(device, vertices.data(), vertices.size());
  }

  void update_vertices(const ComPtr<ID3D11DeviceContext> &context,
                       const void *p, size_t size);

  template <typename T>
  void update_vertices(const ComPtr<ID3D11DeviceContext> &context, const T &t) {
    update_vertices(context, &t, sizeof(T));
  }

  // indices
  bool create_dynamic_indices(const ComPtr<ID3D11Device> &device, size_t stride,
                              size_t dynamic_size);

  bool create_indices(const ComPtr<ID3D11Device> &device, size_t stride,
                      const void *p, size_t size);

  template <typename T>
  bool create_indices(const ComPtr<ID3D11Device> &device, const T *p,
                      size_t count) {
    auto stride = sizeof(T);
    return create_indices(device, stride, p, stride * count);
  }

  template <typename T>
  bool create_indices(const ComPtr<ID3D11Device> &device,
                      const std::vector<T> &indices) {
    return create_indices(device, indices.data(), indices.size());
  }

  void update_indices(const ComPtr<ID3D11DeviceContext> &context, const void *p,
                      size_t size);

  // draw
  void setup(const ComPtr<ID3D11DeviceContext> &context);
  void draw_submesh(const ComPtr<ID3D11DeviceContext> &context, UINT offset,
                    UINT count);
  void draw(const ComPtr<ID3D11DeviceContext> &context);
};

} // namespace gorilla
