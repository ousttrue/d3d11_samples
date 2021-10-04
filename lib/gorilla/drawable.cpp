#include "drawable.h"

namespace gorilla {

State::State() {
  rs_desc.CullMode = D3D11_CULL_BACK;
  rs_desc.FillMode = D3D11_FILL_SOLID;
  rs_desc.FrontCounterClockwise = true;
  rs_desc.ScissorEnable = false;
  rs_desc.MultisampleEnable = false;

  bs_desc.AlphaToCoverageEnable = FALSE;
  bs_desc.IndependentBlendEnable = FALSE;
  bs_desc.RenderTarget[0].BlendEnable = TRUE;
  bs_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
  bs_desc.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
  bs_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  bs_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
  bs_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
  bs_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
  bs_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

bool State::create(const ComPtr<ID3D11Device> &device,
                   bool create_blend_state) {
  if (FAILED(device->CreateRasterizerState(&rs_desc, &rs))) {
    return {};
  }
  if (create_blend_state) {
    if (FAILED(device->CreateBlendState(&bs_desc, &bs))) {
      return {};
    }
  }
  return true;
}

void State::setup(const ComPtr<ID3D11DeviceContext> &context) {
  context->RSSetState(rs.Get());
  context->OMSetBlendState(bs.Get(), blend_factor, 0xffffffff);
}

void Drawable::draw(const ComPtr<ID3D11DeviceContext> &context) {
  state.setup(context);
  pipeline.setup(context);
  if (ia.is_enable()) {
    ia.draw(context);
  } else {
    pipeline.draw_empty(context);
  }
}

void Drawable::draw(const ComPtr<ID3D11DeviceContext> &context,
                    const banana::OrbitCamera &camera,
                    std::span<const banana::LightInfo> lights) {
  // update backing store
  pipeline.vs_stage.set_variables(camera);
  pipeline.gs_stage.set_variables(camera);
  pipeline.ps_stage.set_variables(camera);
  // backing store to GPU
  pipeline.update(context);
  // draw
  draw(context);
}

} // namespace gorilla