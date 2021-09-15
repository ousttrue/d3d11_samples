#include "constant_buffer.h"
#include <assert.h>

namespace gorilla {

bool ConstantBuffer::create(const ComPtr<ID3D11Device> &device, UINT size) {
  desc.ByteWidth = size;
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  auto hr = device->CreateBuffer(&desc, nullptr, &buffer);
  if (FAILED(hr)) {
    return false;
  }
  return true;
}

void ConstantBuffer::update(const ComPtr<ID3D11DeviceContext> &context,
                            const void *p, UINT size) {
  assert(desc.ByteWidth == size);
  context->UpdateSubresource(buffer.Get(), 0, nullptr, p, 0, 0);
}

} // namespace gorilla
