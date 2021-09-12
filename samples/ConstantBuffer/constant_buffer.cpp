#include "constant_buffer.h"
#include <assert.h>

namespace gorilla {

bool ConstantBuffer::create(const ComPtr<ID3D11Device> &device, UINT size) {
  _desc.ByteWidth = size;
  _desc.Usage = D3D11_USAGE_DEFAULT;
  _desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  auto hr = device->CreateBuffer(&_desc, nullptr, &_buffer);
  if (FAILED(hr)) {
    return false;
  }
  return true;
}

void ConstantBuffer::update(const ComPtr<ID3D11DeviceContext> &context,
                            const void *p, UINT size) {
  assert(_desc.ByteWidth == size);
  context->UpdateSubresource(_buffer.Get(), 0, nullptr, p, 0, 0);
}

void ConstantBuffer::set_gs(const ComPtr<ID3D11DeviceContext> &context,
                            int slot) {
  ID3D11Buffer *cbs[1] = {_buffer.Get()};
  context->GSSetConstantBuffers(slot, 1, cbs);
}

} // namespace gorilla
