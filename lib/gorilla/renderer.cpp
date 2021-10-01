#pragma once
#include "renderer.h"
#include "device.h"
#include "swapchain.h"
#include <assert.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace gorilla {

std::tuple<ComPtr<ID3D11Device>, ComPtr<ID3D11DeviceContext>>
Renderer::create(HWND hwnd) {
  _device = gorilla::create_device();
  if (!_device) {
    return {};
  }
  _device->GetImmediateContext(&_context);

  _swapchain = gorilla::create_swapchain(_device, hwnd);
  if (!_swapchain) {
    return {};
  }
  _swapchain->GetDesc(&_desc);

  return {_device, _context};
}

void Renderer::begin_frame(const ScreenState &state, const float clear[4]) {
  if (state.width != _desc.BufferDesc.Width ||
      state.height != _desc.BufferDesc.Height) {
    // clear backbuffer reference
    _render_target.release();
    // resize swapchain
    _swapchain->ResizeBuffers(_desc.BufferCount, static_cast<UINT>(state.width),
                              static_cast<UINT>(state.height),
                              _desc.BufferDesc.Format, _desc.Flags);
  }

  // ensure create backbuffer
  if (!_render_target.get()) {
    ComPtr<ID3D11Texture2D> backbuffer;
    auto hr = _swapchain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));
    if (FAILED(hr)) {
      assert(false);
    }

    if (!_render_target.create_rtv(_device, backbuffer)) {
      assert(false);
    }
    if (!_render_target.create_dsv(_device)) {
      assert(false);
    }
  }

  _render_target.clear(_context, clear);
  _render_target.setup(_context, state.width, state.height);
}

void Renderer::end_frame() {
  // vsync
  _context->Flush();
  _swapchain->Present(1, 0);
}

void Renderer::clear_depth() { _render_target.clear_depth(_context); }

void Renderer::render(const ComPtr<ID3D11DeviceContext> &context,
                      gorilla::Drawable *drawable,
                      const banana::OrbitCamera &camera,
                      std::span<const banana::LightInfo> lights) {

  drawable->pipeline.gs_stage.set_variable(
      banana::Semantics::CAMERA_NEAR_FAR_FOVY,
      banana::Float3{camera._near, camera._far, camera.fovYRad});
  drawable->pipeline.gs_stage.set_variable(
      banana::Semantics::CURSOR_SCREEN_SIZE,
      banana::Float4{0, 0, camera.screen.x, camera.screen.y});
  drawable->pipeline.gs_stage.set_variable(banana::Semantics::CAMERA_VIEW,
                                           camera.view);
  drawable->pipeline.gs_stage.set_variable(banana::Semantics::CAMERA_PROJECTION,
                                           camera.projection);
  auto p = camera.position();
  drawable->pipeline.gs_stage.set_variable(banana::Semantics::CAMERA_POSITION,
                                           banana::Float3{p.x, p.y, p.z});

  drawable->pipeline.ps_stage.set_variable(
      banana::Semantics::CAMERA_NEAR_FAR_FOVY,
      banana::Float3{camera._near, camera._far, camera.fovYRad});
  drawable->pipeline.ps_stage.set_variable(
      banana::Semantics::CURSOR_SCREEN_SIZE,
      banana::Float4{0, 0, camera.screen.x, camera.screen.y});
  drawable->pipeline.ps_stage.set_variable(banana::Semantics::CAMERA_VIEW,
                                           camera.view);
  drawable->pipeline.ps_stage.set_variable(banana::Semantics::CAMERA_PROJECTION,
                                           camera.projection);
  drawable->pipeline.ps_stage.set_variable(banana::Semantics::CAMERA_POSITION,
                                           banana::Float3{p.x, p.y, p.z});

  drawable->pipeline.update(context);
  drawable->draw(context);
}

} // namespace gorilla
