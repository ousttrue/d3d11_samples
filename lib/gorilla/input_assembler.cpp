#include "input_assembler.h"
#include <assert.h>
#include <stdexcept>

namespace gorilla {

bool InputAssembler::create_dynamic_vertices(const ComPtr<ID3D11Device> &device,
                                             size_t stride,
                                             size_t dynamic_size) {
  _vdesc.ByteWidth = static_cast<UINT>(dynamic_size);
  _vdesc.Usage = D3D11_USAGE_DYNAMIC;
  _vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  _vdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  auto hr = device->CreateBuffer(&_vdesc, nullptr, &_vb);
  if (FAILED(hr)) {
    return false;
  }

  _vertex_stride = static_cast<UINT>(stride);
  return true;
}

bool InputAssembler::create_vertices(const ComPtr<ID3D11Device> &device,
                                     size_t stride, const void *p,
                                     size_t size) {
  _vdesc.ByteWidth = static_cast<UINT>(size);
  _vdesc.Usage = D3D11_USAGE_DEFAULT;
  _vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  _vdesc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA data = {0};
  data.pSysMem = p;

  auto hr = device->CreateBuffer(&_vdesc, &data, &_vb);
  if (FAILED(hr)) {
    return false;
  }

  _vertex_count = static_cast<UINT>(size / stride);
  _vertex_stride = static_cast<UINT>(stride);
  return true;
}

void InputAssembler::update_vertices(const ComPtr<ID3D11DeviceContext> &context,
                                     const void *p, size_t size) {

  assert(_vdesc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE);
  if (static_cast<UINT>(size) > _vdesc.ByteWidth) {
    assert(false);
  }

  D3D11_MAPPED_SUBRESOURCE mapped;
  auto hr = context->Map(_vb.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
  if (SUCCEEDED(hr)) {
    memcpy(mapped.pData, p, size);
    context->Unmap(_vb.Get(), 0);
    _vertex_count = static_cast<UINT>(size / _vertex_stride);
  }
}

static DXGI_FORMAT size_to_format(size_t size) {
  switch (size) {
  case 2:
    return DXGI_FORMAT_R16_UINT;
  case 4:
    return DXGI_FORMAT_R32_UINT;
  default:
    throw std::runtime_error(
        "only DXGI_FORMAT_R16_UINT or DXGI_FORMAT_R32_UINT is allowed");
  }
}

bool InputAssembler::create_dynamic_indices(const ComPtr<ID3D11Device> &device,
                                            size_t stride,
                                            size_t dynamic_size) {
  _idesc.ByteWidth = static_cast<UINT>(dynamic_size);
  _idesc.Usage = D3D11_USAGE_DYNAMIC;
  _idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  _idesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

  auto hr = device->CreateBuffer(&_idesc, nullptr, &_ib);
  if (FAILED(hr)) {
    return false;
  }
  _index_stride = static_cast<UINT>(stride);
  _index_format = size_to_format(stride);
  return true;
}

bool InputAssembler::create_indices(const ComPtr<ID3D11Device> &device,
                                    size_t stride, const void *p, size_t size) {
  _idesc.ByteWidth = static_cast<UINT>(size);
  _idesc.Usage = D3D11_USAGE_DEFAULT;
  _idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  _idesc.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA data = {0};
  data.pSysMem = p;

  auto hr = device->CreateBuffer(&_idesc, &data, &_ib);
  if (FAILED(hr)) {
    return false;
  }
  _index_count = static_cast<UINT>(size / stride);
  _index_stride = static_cast<UINT>(stride);
  _index_format = size_to_format(stride);
  return true;
}

void InputAssembler::update_indices(const ComPtr<ID3D11DeviceContext> &context,
                                    const void *p, size_t size) {
  assert(_idesc.CPUAccessFlags & D3D11_CPU_ACCESS_WRITE);
  if (static_cast<UINT>(size) > _idesc.ByteWidth) {
    assert(false);
  }

  D3D11_MAPPED_SUBRESOURCE mapped;
  auto hr = context->Map(_ib.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
  if (SUCCEEDED(hr)) {
    memcpy(mapped.pData, p, size);
    context->Unmap(_ib.Get(), 0);
    _index_count = static_cast<UINT>(size / _index_stride);
  }
}

void InputAssembler::setup(const ComPtr<ID3D11DeviceContext> &context) {

  // set vertexbuffer
  UINT offset = 0;
  context->IASetVertexBuffers(0, 1, _vb.GetAddressOf(), &_vertex_stride,
                              &offset);
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  context->IASetIndexBuffer(_ib.Get(), _index_format, 0);
}

void InputAssembler::draw_submesh(const ComPtr<ID3D11DeviceContext> &context,
                                  UINT offset, UINT count) {
  if (_ib) {
    context->DrawIndexed(count, offset, 0);
  } else {
    context->Draw(count, offset);
  }
}

void InputAssembler::draw(const ComPtr<ID3D11DeviceContext> &context) {
  setup(context);
  if (_ib) {
    draw_submesh(context, 0, _index_count);
  } else {
    draw_submesh(context, 0, _vertex_count);
  }
}

} // namespace gorilla
