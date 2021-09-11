#include "device.h"

namespace swtk {

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

ComPtr<ID3D11Device> create_device(const ComPtr<IDXGIAdapter> &adapter) {
  D3D_DRIVER_TYPE dtype = D3D_DRIVER_TYPE_HARDWARE;
  UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT; // for D2D
#ifdef _DEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
  D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_0};
  UINT numFeatureLevels = sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL);
  UINT sdkVersion = D3D11_SDK_VERSION;
  D3D_FEATURE_LEVEL validFeatureLevel;
  ComPtr<ID3D11Device> device;
  HRESULT hr = D3D11CreateDevice(adapter.Get(), dtype, nullptr, flags,
                                 featureLevels, numFeatureLevels, sdkVersion,
                                 &device, &validFeatureLevel, nullptr);
  if (FAILED(hr)) {
    return nullptr;
  }
  if (validFeatureLevel != featureLevels[0]) {
    return nullptr;
  }

  return device;
}

} // namespace swtk
