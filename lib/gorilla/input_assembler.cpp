#include "input_assembler.h"
#include <assert.h>
#include <stdexcept>

namespace gorilla {

bool InputAssembler::create_vertices(const ComPtr<ID3D11Device> &device,
                                     size_t stride, const void *p, size_t size,
                                     size_t dynamic_size) {

  if (dynamic_size) {
    assert(size <= dynamic_size);
    _vdesc.ByteWidth = static_cast<UINT>(dynamic_size);
    _vdesc.Usage = D3D11_USAGE_DYNAMIC;
    _vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _vdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  } else {
    _vdesc.ByteWidth = static_cast<UINT>(size);
    _vdesc.Usage = D3D11_USAGE_DEFAULT;
    _vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    _vdesc.CPUAccessFlags = 0;
  }

  D3D11_SUBRESOURCE_DATA data = {0};
  data.pSysMem = p;

  auto hr = device->CreateBuffer(&_vdesc, &data, &_pVertexBuf);
  if (FAILED(hr)) {
    return false;
  }

  _vertex_count = static_cast<UINT>(size / stride);
  _strides[0] = static_cast<UINT>(stride);

  return true;
}

void InputAssembler::update_vertices(const ComPtr<ID3D11DeviceContext> &context,
                                     const void *p, int size) {

  assert(_vdesc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE);
  if (static_cast<UINT>(size) <= _vdesc.ByteWidth) {
    D3D11_MAPPED_SUBRESOURCE mapped;
    auto hr =
        context->Map(_pVertexBuf.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    if (SUCCEEDED(hr)) {
      memcpy(mapped.pData, p, size);
      context->Unmap(_pVertexBuf.Get(), 0);
      _vertex_count = static_cast<UINT>(size / _strides[0]);
    }
  } else {
    assert(false);
  }
}

bool InputAssembler::create_indices(const ComPtr<ID3D11Device> &device,
                                    size_t stride, const void *p, size_t size) {
  _idesc.ByteWidth = static_cast<UINT>(size);
  _idesc.Usage = D3D11_USAGE_DEFAULT;
  _idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  _idesc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA data = {0};
  data.pSysMem = p;

  auto hr = device->CreateBuffer(&_idesc, &data, &_pIndexBuf);
  if (FAILED(hr)) {
    return false;
  }
  _index_count = static_cast<UINT>(size / stride);

  switch (stride) {
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

void InputAssembler::update_indices(const ComPtr<ID3D11DeviceContext> &context,
                                    const void *p, int size) {
  throw std::runtime_error("not implemented");
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
