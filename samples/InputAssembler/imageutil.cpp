#include "imageutil.h"
#include <wincodec.h>
#include <assert.h>

namespace imageutil {

Image::Image(int w, int h, int pixelBytes)
    : m_width(w), m_height(h), m_pixelBytes(pixelBytes)
    , m_buffer(w*h*pixelBytes)
{
}


Factory::Factory()
	: m_factory(nullptr)
{
    HRESULT hr = CoCreateInstance(
        CLSID_WICImagingFactory,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&m_factory)
        );
}

Factory::~Factory()
{
	if (m_factory){
		m_factory->Release();
		m_factory = nullptr;
	}
}

std::shared_ptr<Image> Factory::Load(const std::wstring &path)
{
    Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
    HRESULT hr = m_factory->CreateDecoderFromFilename(path.c_str(), 0
            , GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
    if(FAILED(hr)){
        return nullptr;
    }

    Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
    hr = decoder->GetFrame(0, &frame);
    if(FAILED(hr)){
        return nullptr;
    }

    UINT width, height;
    hr = frame->GetSize( &width, &height );
    if(FAILED(hr)){
        return nullptr;
    }

    assert( width > 0 && height > 0 );

	// Determine format
	WICPixelFormatGUID pixelFormat;
	hr = frame->GetPixelFormat(&pixelFormat);
	if (FAILED(hr)){
		return nullptr;
	}

	if (pixelFormat != GUID_WICPixelFormat32bppRGBA){
        Microsoft::WRL::ComPtr<IWICFormatConverter> FC;
        hr = m_factory->CreateFormatConverter(&FC);
        if(FAILED(hr)){
            return nullptr;
        }

        hr = FC->Initialize(frame.Get(), GUID_WICPixelFormat32bppRGBA
                , WICBitmapDitherTypeErrorDiffusion
                , 0, 0, WICBitmapPaletteTypeCustom);
        if(FAILED(hr)){
            return nullptr;
        }

		// copy
		auto image=std::make_shared<Image>(width, height, 4);
		FC->CopyPixels(0, image->Stride(), (UINT)image->Size(), image->Pointer());
		return image;
	}
	else{
		// copy
		auto image = std::make_shared<Image>(width, height, 4);
		frame->CopyPixels(0, image->Stride(), (UINT)image->Size(), image->Pointer());
		return image;
	}

	return nullptr;
}

} // namespace
