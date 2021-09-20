#include "render_target.h"

namespace gorilla {

bool RenderTarget::create_rtv(const ComPtr<ID3D11Device> &device,
                              const ComPtr<ID3D11Texture2D> &texture) {
  auto hr = device->CreateRenderTargetView(texture.Get(), nullptr, &_rtv);
  if (FAILED(hr)) {
    return false;
  }

  texture->GetDesc(&_desc);
  return true;
}

bool RenderTarget::create_dsv(const ComPtr<ID3D11Device> &device) {

  D3D11_TEXTURE2D_DESC depthDesc = {0};
  depthDesc.Width = _desc.Width;
  depthDesc.Height = _desc.Height;
  depthDesc.MipLevels = 1;
  depthDesc.ArraySize = 1;
  depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthDesc.SampleDesc.Count = 1;
  depthDesc.SampleDesc.Quality = 0;
  depthDesc.Usage = D3D11_USAGE_DEFAULT;
  depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthDesc.CPUAccessFlags = 0;
  depthDesc.MiscFlags = 0;
  auto hr = device->CreateTexture2D(&depthDesc, NULL, &_depthStencil);
  if (FAILED(hr)) {
    return false;
  }

  D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
  dsvDesc.Format = depthDesc.Format;
  dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
  dsvDesc.Texture2D.MipSlice = 0;
  hr = device->CreateDepthStencilView(_depthStencil.Get(), &dsvDesc, &_dsv);
  if (FAILED(hr)) {
    return false;
  }

  D3D11_DEPTH_STENCIL_DESC dssDesc = {0};
  dssDesc.DepthEnable = true;
  dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
  dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dssDesc.StencilEnable = false;

  return true;
}

void RenderTarget::clear(const ComPtr<ID3D11DeviceContext> &context,
                         const float clear[4]) {
  context->ClearRenderTargetView(_rtv.Get(), clear);

  if (_dsv) {
    float clearDepth = 1.0f;
    context->ClearDepthStencilView(
        _dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, 0);
  }
}

void RenderTarget::setup(const ComPtr<ID3D11DeviceContext> &context, float w,
                         float h) {
  // set backbuffer & depthbuffer
  ID3D11RenderTargetView *rtv_list[] = {_rtv.Get()};
  context->OMSetRenderTargets(1, rtv_list, _dsv.Get());
  context->OMSetDepthStencilState(_dss.Get(), 0);

  D3D11_VIEWPORT viewports[1] = {{0}};
  viewports[0].TopLeftX = 0;
  viewports[0].TopLeftY = 0;
  viewports[0].Width = w;
  viewports[0].Height = h;
  viewports[0].MinDepth = 0;
  viewports[0].MaxDepth = 1.0f;
  context->RSSetViewports(_countof(viewports), viewports);
}

} // namespace gorilla
