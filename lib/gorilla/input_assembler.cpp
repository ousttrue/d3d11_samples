#include "input_assembler.h"

namespace gorilla {

bool InputAssembler::create_vertices(const ComPtr<ID3D11Device> &device,
                                     const void *p, size_t size, size_t count) {

  D3D11_BUFFER_DESC desc = {0};
  desc.ByteWidth = static_cast<UINT>(size);
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  desc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA data = {0};
  data.pSysMem = p;

  auto hr = device->CreateBuffer(&desc, &data, &_pVertexBuf);
  if (FAILED(hr)) {
    return false;
  }

  _vertex_count = static_cast<UINT>(count);
  _strides[0] = static_cast<UINT>(size / count);

  return true;
}

bool InputAssembler::create_indices(const ComPtr<ID3D11Device> &device,
                                    const void *p, size_t size, size_t count) {
  D3D11_BUFFER_DESC desc = {0};
  desc.ByteWidth = static_cast<UINT>(size);
  desc.Usage = D3D11_USAGE_DEFAULT;
  desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  desc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA data = {0};
  data.pSysMem = p;

  auto hr = device->CreateBuffer(&desc, &data, &_pIndexBuf);
  if (FAILED(hr)) {
    return false;
  }
  _index_count = static_cast<UINT>(count);

  switch (size / count) {
  case 1:
    _index_format = DXGI_FORMAT_R8_UINT;
    break;
  case 2:
    _index_format = DXGI_FORMAT_R16_UINT;
    break;
  case 4:
    _index_format = DXGI_FORMAT_R32_UINT;
    break;
  default:
    return false;
  }

  return true;
}

void InputAssembler::setup(const ComPtr<ID3D11DeviceContext> &context) {

  // set vertexbuffer
  ID3D11Buffer *pBufferTbl[] = {_pVertexBuf.Get()};
  UINT OffsetTbl[] = {0};
  context->IASetVertexBuffers(0, 1, pBufferTbl, _strides, OffsetTbl);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  if (_pIndexBuf) {
    context->IASetIndexBuffer(_pIndexBuf.Get(), _index_format, 0);
  }
}

void InputAssembler::draw_submesh(const ComPtr<ID3D11DeviceContext> &context,
                                  UINT offset, UINT count) {
  if (_pIndexBuf) {
    context->DrawIndexed(count, offset, 0);
  } else {
    context->Draw(count, offset);
  }
}

void InputAssembler::draw(const ComPtr<ID3D11DeviceContext> &context) {
  setup(context);
  if (_pIndexBuf) {
    draw_submesh(context, 0, _index_count);
  } else {
    draw_submesh(context, 0, _vertex_count);
  }
}

} // namespace gorilla
