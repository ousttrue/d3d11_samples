#include "image.h"
#include <assert.h>
#include <shlwapi.h>
#include <wincodec.h>
#include <wrl/client.h>

template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

static ComPtr<IWICImagingFactory> create_factory() {
  ComPtr<IWICImagingFactory> factory;
  auto hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr,
                             CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory));
  if (FAILED(hr)) {
    return {};
  }
  return factory;
}

const auto RGBA_FORMAT = GUID_WICPixelFormat32bppRGBA;

static ComPtr<IWICBitmapSource>
from_bytes(const ComPtr<IWICImagingFactory> &factory,
           const ComPtr<IStream> &stream) {

  ComPtr<IWICBitmapDecoder> decoder;
  auto hr = factory->CreateDecoderFromStream(
      stream.Get(), nullptr, WICDecodeMetadataCacheOnDemand, &decoder);
  if (FAILED(hr)) {
    return {};
  }

  ComPtr<IWICBitmapFrameDecode> frame;
  hr = decoder->GetFrame(0, &frame);
  if (FAILED(hr)) {
    return {};
  }

  // Determine format
  WICPixelFormatGUID pixelFormat;
  hr = frame->GetPixelFormat(&pixelFormat);
  if (FAILED(hr)) {
    return {};
  }

  if (pixelFormat == RGBA_FORMAT) {
    return frame;
  }

  Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
  hr = factory->CreateFormatConverter(&FC);
  if (FAILED(hr)) {
    return {};
  }

  BOOL can_convert;
  hr = FC->CanConvert(pixelFormat, RGBA_FORMAT, &can_convert);
  if(FAILED(hr))
  {
    return {};
  }
  if(!can_convert)
  {
    return {};
  }

  hr = FC->Initialize(frame.Get(), RGBA_FORMAT,
                      WICBitmapDitherTypeErrorDiffusion, 0, 0,
                      WICBitmapPaletteTypeCustom);
  if (FAILED(hr)) {
    return {};
  }

  return FC;
}

namespace banana {

bool Image::load(std::span<const uint8_t> bytes) {

  auto factory = create_factory();

  ComPtr<IStream> stream =
      SHCreateMemStream(bytes.data(), static_cast<UINT>(bytes.size()));
  if (!stream) {
    return {};
  }

  auto frame = from_bytes(factory, stream);
  if (!frame) {
    return {};
  }

  auto hr = frame->GetSize(&_width, &_height);
  if (FAILED(hr)) {
    return {};
  }

  WICPixelFormatGUID pixel_format;
  hr = frame->GetPixelFormat(&pixel_format);
  assert(pixel_format == RGBA_FORMAT);

  _buffer.resize(_width * _height * 4);

#if 0
  // flip vertical
  ComPtr<IWICBitmapFlipRotator> pFlipRotator;
  hr = factory->CreateBitmapFlipRotator(&pFlipRotator);
  if (FAILED(hr)) {
    return {};
  }
  hr = pFlipRotator->Initialize(frame.Get(), WICBitmapTransformFlipVertical);
  if (FAILED(hr)) {
    return {};
  }
  hr = pFlipRotator->CopyPixels(0, _width * 4, (UINT)_buffer.size(),
                                _buffer.data());
  if (FAILED(hr)) {
    return {};
  }
#else
  hr = frame->CopyPixels(0, _width * 4, (UINT)_buffer.size(), _buffer.data());
  if (FAILED(hr)) {
    return {};
  }
#endif

  return true;
}

} // namespace banana