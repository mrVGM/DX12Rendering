#include "ImageLoader.h"

#include "ImageLoaderMeta.h"

#include "CoreUtils.h"

#include "DXTexture.h"
#include "DXHeap.h"
#include "DXBuffer.h"
#include "DXBufferMeta.h"

#include "ImageTextureMeta.h"

#include "DataLib.h"

#include "d3dx12.h"

#include <d3d12.h>

#define THROW_ERROR(hRes, error) \
if (FAILED(hRes)) {\
    throw error;\
}

namespace
{
	rendering::DXDevice* m_device = nullptr;

	void CacheObjects()
	{
		using namespace rendering;

		if (!m_device)
		{
			m_device = core::utils::GetDevice();
		}
	}

	// get the dxgi format equivilent of a wic format
	DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wicFormatGUID)
	{
		if (wicFormatGUID == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (wicFormatGUID == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (wicFormatGUID == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (wicFormatGUID == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		else return DXGI_FORMAT_UNKNOWN;
	}

	// get the number of bits per pixel for a dxgi format
	int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgiFormat)
	{
		if (dxgiFormat == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
		else if (dxgiFormat == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

		else if (dxgiFormat == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
		else if (dxgiFormat == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_B5G6R5_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R32_FLOAT) return 32;
		else if (dxgiFormat == DXGI_FORMAT_R16_FLOAT) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R16_UNORM) return 16;
		else if (dxgiFormat == DXGI_FORMAT_R8_UNORM) return 8;
		else if (dxgiFormat == DXGI_FORMAT_A8_UNORM) return 8;
	}
}

rendering::image_loading::ImageLoader::ImageLoader() :
	BaseObject(ImageLoaderMeta::GetInstance())
{
	CacheObjects();

	THROW_ERROR(
		CoInitialize(nullptr),
		"CoInitialize Error!"
	)

	THROW_ERROR(
		CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IWICImagingFactory,
			reinterpret_cast<LPVOID*>(m_factory.GetAddressOf())
		),
		"Can't Create WIC Imaging Factory!"
	)
}

rendering::image_loading::ImageLoader::~ImageLoader()
{
}

void rendering::image_loading::ImageLoader::LoadImageFromFile(const std::string& imageFile, jobs::Job* done)
{
	std::string fullPath = data::GetLibrary().GetRootDir() + imageFile;
	std::wstring imageFileW(fullPath.begin(), fullPath.end());

	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;

	HRESULT hr = m_factory->CreateDecoderFromFilename(
		imageFileW.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		&decoder
	);

	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
	hr = decoder->GetFrame(0, &frame);

	WICPixelFormatGUID pf;
	hr = frame->GetPixelFormat(&pf);

	DXGI_FORMAT format = GetDXGIFormatFromWICFormat(pf);

	UINT width;
	UINT height;
	frame->GetSize(&width, &height);
	
	struct Context
	{
		std::string m_imageName;

		int m_loading = 2;

		int m_width = -1;
		int m_height = -1;
		DXGI_FORMAT m_format = DXGI_FORMAT_UNKNOWN;

		DXHeap* m_heap = nullptr;
		DXTexture* m_texture = nullptr;

		DXHeap* m_bufferHeap = nullptr;
		DXBuffer* m_buffer = nullptr;

		jobs::Job* m_done = nullptr;
	};

	class PlaceResources : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		PlaceResources(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			m_ctx.m_texture->Place(*m_ctx.m_heap, 0);
			m_ctx.m_buffer->Place(m_ctx.m_bufferHeap, 0);


			core::utils::RunSync(m_ctx.m_done);
		}
	};

	class HeapResident : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		HeapResident(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			--m_ctx.m_loading;
			if (m_ctx.m_loading > 0)
			{
				return;
			}

			core::utils::RunAsync(new PlaceResources(m_ctx));
		}
	};

	class CreateItems : public jobs::Job
	{
	private:
		Context& m_ctx;
	public:
		CreateItems(Context& ctx) :
			m_ctx(ctx)
		{
		}

		void Do() override
		{
			D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAGS::D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
			CD3DX12_RESOURCE_DESC textureDesc = {};
			textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
				m_ctx.m_format,
				m_ctx.m_width,
				m_ctx.m_height,
				1,
				0,
				1,
				0,
				flags);


			m_ctx.m_texture = new DXTexture(ImageTextureMeta::GetInstance(), textureDesc);

			m_ctx.m_heap = new DXHeap();
			m_ctx.m_heap->SetHeapType(D3D12_HEAP_TYPE_DEFAULT);
			m_ctx.m_heap->SetHeapSize(m_ctx.m_texture->GetTextureAllocationInfo().SizeInBytes);
			m_ctx.m_heap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES);
			m_ctx.m_heap->Create();

			UINT64 textureBufferSize;
			m_device->GetDevice().GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureBufferSize);

			m_ctx.m_buffer = new DXBuffer(DXBufferMeta::GetInstance());
			m_ctx.m_buffer->SetBufferSizeAndFlags(textureBufferSize, D3D12_RESOURCE_FLAG_NONE);
			m_ctx.m_buffer->SetBufferStride(textureBufferSize);

			m_ctx.m_bufferHeap = new DXHeap();
			m_ctx.m_bufferHeap->SetHeapType(D3D12_HEAP_TYPE_UPLOAD);
			m_ctx.m_bufferHeap->SetHeapSize(textureBufferSize);
			m_ctx.m_bufferHeap->SetHeapFlags(D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS);
			m_ctx.m_bufferHeap->Create();

			m_ctx.m_heap->MakeResident(new HeapResident(m_ctx));
			m_ctx.m_bufferHeap->MakeResident(new HeapResident(m_ctx));
		}
	};


}

#undef THROW_ERROR