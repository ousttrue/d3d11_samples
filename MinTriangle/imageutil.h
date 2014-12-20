	#pragma once
	#include <memory>
	#include <string>
	#include <vector>
	#include <wrl/client.h>


	struct IWICImagingFactory;

	namespace imageutil {

	class Image
	{
		std::vector<unsigned char> m_buffer;
		int m_width;
		int m_height;
		int m_pixelBytes;

	public:
		Image(int w, int h, int pixelBytes);
		int Width()const{ return m_width; }
		int Stride()const{ return m_width*m_pixelBytes; }
		int Height()const{ return m_height; }
		unsigned char* Pointer(){ return m_buffer.empty() ? nullptr : &m_buffer[0]; }
		size_t Size()const{ return m_buffer.size(); }
	};

	class Factory
	{
		IWICImagingFactory *m_factory;

	public:
		Factory();
		~Factory();
		std::shared_ptr<Image> Load(const std::wstring &path);
	};

	}
