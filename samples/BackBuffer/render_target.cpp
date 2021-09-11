#include "render_target.h"

namespace swtk {

void RenderTarget::clear(const ComPtr<ID3D11DeviceContext> &context,
                         const float clear[4]) {
  context->ClearRenderTargetView(_rtv.Get(), clear);

  if (_dsv) {
    float clearDepth = 1.0f;
    context->ClearDepthStencilView(
        _dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, 0);
  }
}
bool RenderTarget::create(const ComPtr<ID3D11Device> &device,
                          const ComPtr<ID3D11Texture2D> &texture,
                          bool create_dsv) {
  auto hr = device->CreateRenderTargetView(texture.Get(), nullptr, &_rtv);
  if (FAILED(hr)) {
    return false;
  }

  if (create_dsv) {
    D3D11_TEXTURE2D_DESC tdesc;
    texture->GetDesc(&tdesc);

    D3D11_TEXTURE2D_DESC depthDesc;
    ZeroMemory(&depthDesc, sizeof(depthDesc));
    depthDesc.Width = tdesc.Width;
    depthDesc.Height = tdesc.Height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthDesc.CPUAccessFlags = 0;
    depthDesc.MiscFlags = 0;
    hr = device->CreateTexture2D(&depthDesc, NULL, &_depthStencil);
    if (FAILED(hr)) {
      return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    dsvDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(_depthStencil.Get(), &dsvDesc, &_dsv);
    if (FAILED(hr)) {
      return false;
    }
  }

  return true;
}

void RenderTarget::setup(const ComPtr<ID3D11DeviceContext> &context, int w,
                         int h) {
  // set backbuffer & depthbuffer
  ID3D11RenderTargetView *rtv_list[] = {_rtv.Get()};
  context->OMSetRenderTargets(1, rtv_list, nullptr);
  D3D11_VIEWPORT viewports[1] = {{0}};
  viewports[0].TopLeftX = 0;
  viewports[0].TopLeftY = 0;
  viewports[0].Width = (float)w;
  viewports[0].Height = (float)h;
  viewports[0].MinDepth = 0;
  viewports[0].MaxDepth = 1.0f;
  context->RSSetViewports(_countof(viewports), viewports);
}

} // namespace swtk
