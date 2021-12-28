#include "texture.h"

namespace gorilla
{

  bool Texture::create(const ComPtr<ID3D11Device> &device, const void *p, UINT w,
                       UINT h)
  {
    desc.Width = w;
    desc.Height = h;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA *pInit = nullptr;
    if (p)
    {
      D3D11_SUBRESOURCE_DATA initData;
      initData.pSysMem = p;
      initData.SysMemPitch = w * 4;
      initData.SysMemSlicePitch = w * h * 4;
      pInit = &initData;
    }
    else{
      // for render target
      desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    }

    auto hr = device->CreateTexture2D(&desc, pInit, &_texture);
    if (FAILED(hr))
    {
      return false;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;

    hr = device->CreateShaderResourceView(_texture.Get(), &SRVDesc, &_srv);
    if (FAILED(hr))
    {
      return false;
    }

    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    hr = device->CreateSamplerState(&samplerDesc, &_sampler);
    if (FAILED(hr))
    {
      return false;
    }

    return true;
  }

  void Texture::set_ps(const ComPtr<ID3D11DeviceContext> &context, UINT srv,
                       UINT sampler)
  {
    context->PSSetShaderResources(srv, 1, _srv.GetAddressOf());
    context->PSSetSamplers(sampler, 1, _sampler.GetAddressOf());
  }

} // namespace gorilla
